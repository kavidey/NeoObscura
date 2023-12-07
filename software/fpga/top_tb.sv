/*
 * Author:    Neil Chulani, nchulani@.hmc.edu
 * Created:   12/6/2023
 * 
 * This file contains a set of modules for QOI compression
 */


module qoi2top (
	input logic sck,
	input logic sdi,
	input logic reset,
	output logic sdo
);
	
	logic spiControllingRam;
	logic [15:0] spiRamIn;
	logic [13:0] spiRamAddress;
	logic spiRamWE;
	logic [15:0] spiRamOut;
	logic pixelsReady;
	logic doneEncoding;
	logic encoderControllingRam;
	logic [15:0] encoderRamIn;
	logic [13:0] encoderRamAddress;
	logic encoderRamWE;
	logic [15:0] encoderRamOut;
	
	logic high_clk;
	HSOSC #()
		 hf_osc (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(high_clk)); 
		
	// Create new clock that is 1/2 the speed of ram clock to remove metastability issues 
	logic [1:0] new_clk;
	always_ff @(negedge high_clk) begin
		new_clk = new_clk + 1;
	end
	
	spi spiInstance (sck, sdi, sdo, reset, spiControllingRam, spiRamIn, spiRamAddress, spiRamWE, spiRamOut, pixelsReady, doneEncoding);
	encoder encoderInstance(new_clk[1], reset, pixelsReady, doneEncoding, encoderControllingRam, encoderRamIn, encoderRamAddress, encoderRamWE, encoderRamOut);
	ram ramInstance (high_clk, spiControllingRam, spiRamIn, spiRamAddress, spiRamWE, spiRamOut, encoderControllingRam, encoderRamIn, encoderRamAddress, encoderRamWE, encoderRamOut);
endmodule

// Module to encode 1200 RGBA pixels in RAM following QOI image compression format
module encoder ( 
	input logic clk,
	input logic reset,
	
	input logic pixelsReady,
	output logic doneEncodingFinal,
	
	output logic encoderControllingRam,
	output logic [15:0] encoderRamIn,
	output logic [13:0] encoderRamAddress,
	output logic encoderRamWE,
	input logic [15:0] encoderRamOut
);
	// Create internal FSM signals for QOI encoding
	logic doneEncoding;
	logic [31:0] lastPixel, curPixel;
	logic [5:0] runCount, index;
	logic [31:0] prevSeen [63:0];
	logic [7:0] encodedByte, curPixelR, curPixelG, curPixelB, curPixelA, lastPixelR, lastPixelG, lastPixelB, lastPixelA;
	logic signed [8:0] curPixelRS, curPixelGS, curPixelBS, lastPixelRS, lastPixelGS, lastPixelBS, vr, vg, vb, vg_r, vg_b;
	logic signed [31:0] vrOutHelper, vgOutHelper, vbOutHelper, vgOutHelper2, vg_rOutHelper, vg_bOutHelper; 
	logic [1:0] vrOut, vgOut, vbOut;
	logic [5:0] vgOut2;
	logic [3:0] vg_rOut, vg_bOut;
	
	assign curPixelR = curPixel[31:24];
	assign curPixelG = curPixel[23:16];
	assign curPixelB = curPixel[15:8];
	assign curPixelA = curPixel[7:0];
	assign lastPixelR = lastPixel[31:24];
	assign lastPixelG = lastPixel[23:16];
	assign lastPixelB = lastPixel[15:8];
	assign lastPixelA = lastPixel[7:0];
	
	assign curPixelRS = curPixelR;
	assign curPixelGS = curPixelG;
	assign curPixelBS = curPixelB;
	assign lastPixelRS = lastPixelR;
	assign lastPixelGS = lastPixelG;
	assign lastPixelBS = lastPixelB;
	
	assign vr = curPixelRS - lastPixelRS;
	assign vg = curPixelGS - lastPixelGS;
	assign vb = curPixelBS - lastPixelBS;
	assign vg_r = vr - vg;
	assign vg_b = vb - vg;
	
	// QOI_OP_DIFF helpers
	assign vrOutHelper = vr + 2;
	assign vrOut = vrOutHelper[1:0];
	assign vgOutHelper = vg + 2;
	assign vgOut = vgOutHelper[1:0];
	assign vbOutHelper = vb + 2;
	assign vbOut = vbOutHelper[1:0];
	
	// QOI_OP_LUMA helpers
	assign vgOutHelper2 = vg + 32;
	assign vgOut2 = vgOutHelper2[5:0];
	assign vg_rOutHelper = vg_r + 8;
	assign vg_rOut = vg_rOutHelper[3:0];
	assign vg_bOutHelper = vg_b + 8;
	assign vg_bOut = vg_bOutHelper[3:0];
	
	// Run count helpers
	logic [5:0] runCountOutHelper;
	assign runCountOutHelper = runCount - 1;
	logic [5:0] runCountOut;
	assign runCountOut = runCountOutHelper[5:0];
	
	assign index = (curPixelR * 3 + curPixelG * 5 + curPixelB * 7 + curPixelA * 11) % 64;
	logic [13:0] readAddress, writeAddress;
	logic finalPixel;
	
	// FSM states
	typedef enum logic [6:0] {S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16, S17, S18, S19, S20, S21, S22, S23, S24, S25, S26, S27, S28, S29, S30, S31, S32, S33, S34, S35, S36, S37, S38, S39, S40, S41, S42, S43, S44, S45, S46, S47, S48, S49, S50, S51, S52, S53, S54, S55, S56, S57, S58, S59, S60, S61, S62, S63, S64, S65, S66, S67, S68, S69, S70, S71, S72, S73, S74, S75, S76, S77, S78, S79, S80, S81, S82, S83, S84, S85, S86, S87, S88, S89, S90, S91, S92, S93, S94, S95, S96, S97, S98, S99, S100} statetype;
	statetype state, nextstate, jumpBack;
	
	// FSM state register
	always_ff @(posedge clk, posedge reset)
		if (reset) state <= S0;
		else	   state <= nextstate;
	
	// FSM nextstate logic
	always_comb
		case (state)
			S0: nextstate = S1;
			S1: if (pixelsReady) nextstate = S3;
				else			 nextstate = S1;
			S2: nextstate = S3; // Start load next pixel
			S3: nextstate = S4;
			S4: nextstate = S5;
			S5: nextstate = S6;
			S6: nextstate = S7;
			S7: nextstate = S8;
			S8: nextstate = S9;
			S9: nextstate = S10;
			S10: nextstate = S11;
			S11: nextstate = S12;
			S12: nextstate = S13;
			S13: nextstate = S14;
			S14: nextstate = S15;
			S15: nextstate = S16;
			S16: nextstate = S17;
			S17: nextstate = S18;
			S18: nextstate = S19;
			S19: nextstate = S20;
			S20: nextstate = S21;
			S21: nextstate = S22;
			S22: nextstate = S23;
			S23: nextstate = S24;
			S24: nextstate = S25;
			S25: nextstate = S26;
			S26: nextstate = S27;
			S27: nextstate = S33; // End load next pixel 
			S28: nextstate = S29; // Begin write encodedByte
			S29: nextstate = S86;
			S86: nextstate = S87;
			S87: nextstate = S30;
			S30: nextstate = S31;
			S31: nextstate = S32; 
			S32: if (doneEncoding) nextstate = S53; // End write encodedByte
				 else nextstate = jumpBack;					 
			S33: if (curPixel == lastPixel) nextstate =  S34; // First encoding state
				 else						 nextstate = S36;
			S34: if (runCount == 62 || finalPixel) nextstate = S35;
				 else								nextstate = S2;
			S35: nextstate = S28;
			S36: if (runCount != 0) nextstate = S37;
				 else		   nextstate = S38;
			S37: nextstate = S28;
			S38: if (curPixel == prevSeen[index]) nextstate = S28;
				 else							   nextstate = S39;
			S39: if (curPixelA == lastPixelA) nextstate = S40;
				 else						   nextstate = S48;
			S40: if (vr > -3 && vr < 2 && vg > -3 && vg < 2 && vb > -3 && vb < 2) nextstate = S28;
				 else						   nextstate = S41;
			S41: if (vg_r > -9 && vg_r < 8 && vg > -33 && vg < 32 && vg_b > -9 && vg_b < 8) nextstate = S42;
				 else nextstate = S44;
			S42: nextstate = S28;
			S43: nextstate = S28;
			S44: nextstate = S28;
			S45: nextstate = S28;
			S46: nextstate = S28;
			S47: nextstate = S28;
			S48: nextstate = S28;
			S49: nextstate = S28;
			S50: nextstate = S28;
			S51: nextstate = S28;
			S52: nextstate = S28; // Last encoding state
			S53: nextstate = S54; // Begin write QOI end footer
			S54: nextstate = S88;
			S88: nextstate = S55;
			S55: nextstate = S56;
			S56: nextstate = S57;
			S57: nextstate = S58;
			S58: nextstate = S89;
			S89: nextstate = S59;
			S59: nextstate = S60;
			S60: nextstate = S61;
			S61: nextstate = S62;
			S62: nextstate = S90;
			S90: nextstate = S63;
			S63: nextstate = S64;
			S64: nextstate = S65;
			S65: nextstate = S66;
			S66: nextstate = S91;
			S91: nextstate = S92;
			S92: nextstate = S68;
			S68: nextstate = S69;
			S69: nextstate = S70;
			S70: nextstate = S93;
			S93: nextstate = S94;
			S94: nextstate = S72;
			S72: nextstate = S73;
			S73: nextstate = S74;
			S74: nextstate = S95;
			S95: nextstate = S96;
			S96: nextstate = S76;
			S76: nextstate = S77;
			S77: nextstate = S78;
			S78: nextstate = S97;
			S97: nextstate = S98;
			S98: nextstate = S80;
			S80: nextstate = S81;
			S81: nextstate = S82;
			S82: nextstate = S99;
			S99: nextstate = S100;
			S100: nextstate = S84;
			S84: nextstate = S85; // End write QOI end footer
			S85: nextstate = S85; // Done
			default: nextstate = S0;
		endcase
	
	// FSM Output logic 
	always_ff @(posedge clk)
		case (state)
			S0: begin
				lastPixel <= 32'h000000FF;
				for (int i = 0; i < 64; i = i + 1) prevSeen[i] <= 0;
				runCount <= 0;
				encodedByte <= 0;
				readAddress <= 0;
				writeAddress <= 4800;
				finalPixel <= 0;
				doneEncoding <= 0;
				doneEncodingFinal <= 0;
			end
			// Nothing in S1, wait for pixelsReady
			S2: lastPixel <= curPixel;
			S3: begin
				encoderRamAddress <= readAddress;
				encoderRamWE <= 0;
			end
			S4: encoderControllingRam <= 1;
			// Nothing in S5, delay to ensure ram output is valid
			S6: curPixel[31:24] <= encoderRamOut[7:0];
			S7: encoderControllingRam <= 0;
			S8: readAddress <= readAddress + 1;
			S9: encoderRamAddress <= readAddress;
			S10: encoderControllingRam <= 1;
			// Nothing in S11, delay to ensure ram output is valid
			S12: curPixel[23:16] <= encoderRamOut[7:0];
			S13: encoderControllingRam <= 0;
			S14: readAddress <= readAddress + 1;
			S15: encoderRamAddress <= readAddress;
			S16: encoderControllingRam <= 1;
			// Nothing in S17, delay to ensure ram output is valid
			S18: curPixel[15:8] <= encoderRamOut[7:0];
			S19: encoderControllingRam <= 0;
			S20: readAddress <= readAddress + 1;
			S21: encoderRamAddress <= readAddress;
			S22: encoderControllingRam <= 1;
			// Nothing in S23, delay to ensure ram output is valid
			S24: curPixel[7:0] <= encoderRamOut[7:0];
			S25: encoderControllingRam <= 0;
			S26: readAddress <= readAddress + 1;
			S27: if (readAddress == 4800) finalPixel <= 1;
			S28: begin
				encoderRamAddress <= writeAddress;
				encoderRamIn[7:0] <= encodedByte; // THIS ONE SHOULD BE encodedBYTE
			end
			S29: encoderControllingRam <= 1;
			S86: encoderRamWE <= 1;
			// Nothing in S30, delay to ensure ram gets written correctly
			S30: encoderRamWE <= 0;
			S31: encoderControllingRam <= 0;
			S32: begin
				encoderRamWE <= 0;
				writeAddress <= writeAddress + 1;
				if (jumpBack == S32) begin
					doneEncoding <= 1;
				end
			end
			S33: if (curPixel == lastPixel) runCount <= runCount + 1;
			// Nothing in S34
			S35: begin
				encodedByte <= {2'b11, runCountOut};
				if (finalPixel) jumpBack <= S32;
				else begin
					jumpBack <= S2;
				end
				runCount <= 0;
			end
			// Nothing in S36
			S37: begin
				encodedByte <= {2'b11, runCountOut};
				jumpBack <= S38;
				runCount <= 0;
			end
			S38: begin
				if (curPixel == prevSeen[index]) begin
					encodedByte <= {2'b00, index};
					if (finalPixel) jumpBack <= S32;
					else			jumpBack <= S2;
				end
				else prevSeen[index] <= curPixel;
			end
			// Nothing in S39
			S40: begin
				if (vr > -3 && vr < 2 && vg > -3 && vg < 2 && vb > -3 && vb < 2) begin
					encodedByte <= {2'b01, vrOut, vgOut, vbOut};
					if (finalPixel) jumpBack <= S32;
					else jumpBack <= S2;
				end
			end
			// Nothing in S41
			S42: begin
				encodedByte <= {2'b10, vgOut2};
				jumpBack <= S43;
			end
			S43: begin
				encodedByte <= {vg_rOut, vg_bOut};
				if (finalPixel) jumpBack <= S32;
				else jumpBack <= S2;
			end
			S44: begin
				encodedByte <= 8'b11111110;
				jumpBack <= S45;
			end
			S45: begin
				encodedByte <= curPixelR;
				jumpBack <= S46;
			end
			S46: begin
				encodedByte <= curPixelG;
				jumpBack <= S47;
			end
			S47: begin
				encodedByte <= curPixelB;
				if (finalPixel) jumpBack <= S32;
				else jumpBack <= S2;
			end
			S48: begin
				encodedByte <= 8'b11111111;
				jumpBack <= S49;
			end
			S49: begin
				encodedByte <= curPixelR;
				jumpBack <= S50;
			end
			S50: begin
				encodedByte <= curPixelG;
				jumpBack <= S51;
			end
			S51: begin
				encodedByte <= curPixelB;
				jumpBack <= S52;
			end
			S52: begin
				encodedByte <= curPixelA;
				if (finalPixel) jumpBack <= S32;
				else jumpBack <= S2;
			end
			S53: begin
				encoderRamAddress <= writeAddress;
				encoderRamIn[7:0] <= 0;
			end
			S54: begin
				encoderControllingRam <= 1;
			end
			S88: encoderRamWE <= 1;
			S55: begin
				encoderRamWE <= 0;
			end
			S56: begin
				encoderControllingRam <= 0;
			end
			S57: begin
				encoderRamAddress <= encoderRamAddress + 1;
			end
			S58: encoderControllingRam <= 1;
			S89: encoderRamWE <= 1;
			S59: begin
				encoderRamWE <= 0;
			end
			S60: encoderControllingRam <= 0;
			S61: begin
				encoderRamAddress <= encoderRamAddress + 1;
			end
			S62: encoderControllingRam <= 1;
			S90: encoderRamWE <= 1;
			S63: begin
				encoderRamWE <= 0;
			end
			S64: encoderControllingRam <= 0;
			S65: begin
				encoderRamAddress <= encoderRamAddress + 1;
			end
			S66: encoderControllingRam <= 1;
			S91: encoderRamWE <= 1;
			S92: encoderRamWE <= 0;
			S68: encoderControllingRam <= 0;
			S69: begin
				encoderRamAddress <= encoderRamAddress + 1;
			end
			S70: encoderControllingRam <= 1;
			S93: encoderRamWE <= 1;
			S94: encoderRamWE <= 0;
			S72: encoderControllingRam <= 0;
			S73: begin
				encoderRamAddress <= encoderRamAddress + 1;
			end
			S74: encoderControllingRam <= 1;
			S95: encoderRamWE <= 1;
			S96: encoderRamWE <= 0;
			S76: encoderControllingRam <= 0;
			S77: begin
				encoderRamAddress <= encoderRamAddress + 1;
			end
			S78: encoderControllingRam <= 1;
			S97: encoderRamWE <= 1;
			S98: encoderRamWE <= 0;
			S80: encoderControllingRam <= 0;
			S81: begin
				encoderRamAddress <= encoderRamAddress + 1;
				encoderRamIn[7:0] <= 1;
			end
			S82: encoderControllingRam <= 1;
			S99: encoderRamWE <= 1;
			S100: encoderRamWE <= 0;
			S84: encoderControllingRam <= 0;
			S85: doneEncodingFinal <= 1;
			
		endcase
endmodule

// Module to recieve 1200 RGBA pixels from MCU and send encoded bytes back to MCU after compression
module spi (
	input logic sck,
	input logic sdi,
	output logic sdo,
	
	input logic reset,
	
	output logic spiControllingRam,
	output logic [15:0] spiRamIn,
	output logic [13:0] spiRamAddress,
	output logic spiRamWE,
	input logic [15:0] spiRamOut,
	
	output logic pixelsReady,
	input logic doneEncoding
);
	logic [2:0] spiOutCounter, spiCounterNew;
	logic [3:0] spiInCounter;
	logic [15:0] dataIn;
	logic firstIn, firstOut;
	logic [7:0] tempPixelData, tempInfoCode, dataOut, tempDataOut, outCode;
	
	assign outCode = 8'h80;
	
	always_ff @(posedge sck) begin
		if (reset) begin
			spiRamAddress = 0;
			pixelsReady = 0;
			firstIn = 1;
			firstOut = 1;
			spiCounterNew = 0;
			sdo = 0;
			spiControllingRam = 0;
			spiRamWE = 0;
			spiRamIn = 0;
		end
		else begin
				if (!pixelsReady) begin
					if (firstIn) begin
						if (spiInCounter == 15) begin
							firstIn = 0;
						end
					end
					else begin
						if (spiInCounter == 0) begin
							tempPixelData = dataIn[15:8];
							tempInfoCode = dataIn[7:0];
							spiControllingRam = 1;
						end
						if (spiInCounter == 1) begin
							spiRamIn[7:0] = tempPixelData;
							spiRamWE = 1;
						end
						if (spiInCounter == 2) begin
							spiControllingRam = 1;
						end
						if (spiInCounter == 5) begin
							spiRamWE = 0;
						end
						if (spiInCounter == 6) begin
							spiControllingRam = 0;
						end
						if (spiInCounter == 7) begin
							spiRamAddress = spiRamAddress + 1;
						end
						if (spiInCounter == 8) begin
							if (tempInfoCode == 8'h08) begin
								pixelsReady = 1;
							end
						end
					end
				end
				if (doneEncoding) begin
					spiCounterNew = spiCounterNew + 1;
					if (firstOut) begin
						// Shift out 0x80 to signal MCU when done encoding
						sdo = outCode[7 - spiOutCounter];
						if (spiOutCounter == 7) begin
							firstOut = 0;
						end
					end
					else begin
						sdo = dataOut[7 - spiOutCounter];
					end
					
					if (spiOutCounter == 0) begin
						spiRamWE = 0;
					end
					if (spiOutCounter == 1) begin
						spiControllingRam = 1;
					end
					if (spiOutCounter == 3) begin
						tempDataOut = spiRamOut[7:0];
					end
					if (spiOutCounter == 4) begin
						spiControllingRam = 0;
					end
					if (spiOutCounter == 5) begin
						spiRamAddress = spiRamAddress + 1;
					end
				end
				else begin
					spiCounterNew = 0;
				end
		end
	end
	
	always_ff @(negedge sck) begin
		if (reset) begin
			spiInCounter = 0;
			spiOutCounter = 0;
			dataIn = 0;
			dataOut = 0;
		end
		else begin
				if (!pixelsReady) begin
					dataIn = {dataIn[14:0], sdi};
					spiInCounter = spiInCounter + 1;
				end
				if (doneEncoding) begin
					spiOutCounter = spiOutCounter + 1;
					if (spiCounterNew == 0) begin
						dataOut = tempDataOut;
					end
				end
				else begin
					spiOutCounter = 0;
				end
			end
	end
endmodule

// Module to allow both SPI and Encoder modules to read/write RAM
module ram (
	input logic clk,
	
	input logic spiControllingRam,
	input logic [15:0] spiRamIn,
	input logic [13:0] spiRamAddress,
	input logic spiRamWE,
	output logic [15:0] spiRamOut,
	
	input logic encoderControllingRam,
	input logic [15:0] encoderRamIn,
	input logic [13:0] encoderRamAddress,
	input logic encoderRamWE,
	output logic [15:0] encoderRamOut
);

	logic WE, CS;
	logic [13:0] AD;
	logic [7:0] DI, DO;
	
	logic [7:0] memory[0:11999];
	
	logic [7:0] output_memory[4800:11999];
	assign output_memory = memory[4800:11999];

	initial $readmemh("image.mem", memory);
	
	always_ff @(posedge clk) if (WE) memory[AD] <= DI;
	
	assign DO = memory[AD];
	
	always_ff @(posedge clk) $writememh("output.mem", output_memory);
	
	

	
	assign CS = 1;
	
	always_ff @(posedge clk) begin
		if (spiControllingRam) begin
			AD <= spiRamAddress;
			DI <= spiRamIn;
			WE <= spiRamWE;
			spiRamOut <= DO;
		end
		if (encoderControllingRam) begin
			AD <= encoderRamAddress;
			DI <= encoderRamIn;
			WE <= encoderRamWE;
			encoderRamOut <= DO;
		end
	end
endmodule


module tb ();
	
	logic high_clk, reset;
		
	logic spiControllingRam;
	logic [15:0] spiRamIn;
	logic [13:0] spiRamAddress;
    logic spiRamWE;
	logic [15:0] spiRamOut;
	
	logic encoderControllingRam;
	logic [15:0] encoderRamIn;
	logic [13:0] encoderRamAddress;
	logic encoderRamWE;
	logic [15:0] encoderRamOut;
	
	 
		 
	always begin
		high_clk = 1; #5;
		high_clk = 0; #5;
	end
	
	initial begin
		reset = 1; #27; reset = 0; 
	end
		
	// Create new clock that is 1/2 the speed of ram clock to remove metastability issues 
	logic [1:0] new_clk;
	always_ff @(negedge high_clk)
		if (reset) new_clk = 0;
		else new_clk = new_clk + 1;
	
	encoder encoderInstance(new_clk[1], reset, 1, doneEncoding, encoderControllingRam, encoderRamIn, encoderRamAddress, encoderRamWE, encoderRamOut);
	ram ramInstance (high_clk, spiControllingRam, spiRamIn, spiRamAddress, spiRamWE, spiRamOut, encoderControllingRam, encoderRamIn, encoderRamAddress, encoderRamWE, encoderRamOut);
endmodule
	