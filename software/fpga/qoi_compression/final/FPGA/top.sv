module qoi_spi (
	input logic spiIn,
	input logic spiOut,
	input logic [7:0] encodedByte,
	input logic sck,
	input logic sdi,
	output logic sdo,
	output logic [31:0] pixel
);

	logic [2:0] spiOutCounter;
	always_ff @(posedge sck) 
		if (spiOut) sdo = encodedByte[7 - spiOutCounter];
	always_ff @(negedge sck) begin
		if (spiOut) spiOutCounter = spiOutCounter + 1;
		else spiOutCounter = 0;
		if (spiIn) pixel = {pixel[30:0], sdi};
	end
endmodule

module qoi_encoder_new (
	input logic reset,
	input logic finalPixel,
	input logic sck, 
	input logic sdi,
	input logic mspi,
	output logic sdo,
	output logic readyForPixel,
	output logic encodedByteReady,
	output logic doneEncoding
);
	logic int_osc, spiIn, spiOut;
    LSOSC #()
         lf_osc (.CLKLFPU(1'b1), .CLKLFEN(1'b1), .CLKLF(int_osc));
	
	logic [31:0] lastPixel, curPixel, delayCounter;
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
	
	logic [5:0] runCountOutHelper;
	assign runCountOutHelper = runCount - 1;
	logic [5:0] runCountOut;
	assign runCountOut = runCountOutHelper[5:0];
	
	assign index = (curPixelR * 3 + curPixelG * 5 + curPixelB * 7 + curPixelA * 11) % 64;
	
	qoi_spi spi(spiIn, spiOut, encodedByte, sck, sdi, sdo, curPixel);
	
	typedef enum logic [5:0] {S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16, S17, S18, S19, S20, S21, S22, S23, S24, S25, S26, S27, S28, S29, S30, S31, S32} statetype;
	statetype state, nextstate, jumpBack;
	
	// State register
	always_ff @(posedge int_osc, posedge reset) 
		if (reset) state <= S0;
		else	   state <= nextstate;
	
	// Next state logic
	always_comb
		case (state)
			S0: nextstate = S2;
			S1: nextstate = S2;
			S2: nextstate = S3;
			S3: nextstate = S5;
			S4: if (delayCounter < 100) nextstate = S4;
				else				   nextstate = S7;
			S5: if (mspi) nextstate = S6;
				else	  nextstate = S5;
			S6: nextstate = S4;
			S7: if (curPixel == lastPixel) nextstate = S8;
				else					  nextstate = S10;
			S8: if (runCount == 62 || finalPixel) nextstate = S9;
				else							 nextstate = S1;
			S9: nextstate = S13;
			S10: if (runCount == 0) nextstate = S12;
				 else		   nextstate = S11;
			S11: nextstate = S13;
			S12: if (curPixel == prevSeen[index]) nextstate = S13;
				 else							nextstate = S19;
			S13: nextstate = S14;
			S14: nextstate = S16;
			S15: if (delayCounter < 100) nextstate = S15;
				 else					nextstate = S18;
			S16: if (mspi) nextstate = S17;
				 else	   nextstate = S16;
			S17: nextstate = S15;
			S18: if (doneEncoding) nextstate = S18;
				 else			 nextstate = jumpBack;
			S19: if (curPixelA == lastPixelA) nextstate = S20;
				 else						 nextstate = S22;
			S20: if (vr > -3 && vr < 2 && vg > -3 && vg < 2 && vb > -3 && vb < 2) nextstate = S13;
				 else nextstate = S21;
			S21: if (vg_r > -9 && vg_r < 8 && vg > -33 && vg < 32 && vg_b > -9 && vg_b < 8) nextstate = S27;
				 else nextstate = S29;
			S22: nextstate = S13;
			S23: nextstate = S13;
			S24: nextstate = S13;
			S25: nextstate = S13;
			S26: nextstate = S13;
			S27: nextstate = S13;
			S28: nextstate = S13;
			S29: nextstate = S13;
			S30: nextstate = S13;
			S31: nextstate = S13;
			S32: nextstate = S13;
			default: nextstate = S0;
		endcase
		
		always_ff @(posedge int_osc) 
			case (state)
				S0: begin
					lastPixel <= 32'h000000FF;
					for (int i = 0; i < 64; i = i + 1) prevSeen[i] <= 0;
					runCount <= 0; 
					encodedByte <= 0;
					spiIn <= 0;
					spiOut <= 0;
					delayCounter <= 0;
					readyForPixel <= 0;
					encodedByteReady <= 0;
					doneEncoding <= 0;
				end
				S1: lastPixel <= curPixel;
				S2: spiIn <= 1;
				S3: readyForPixel <= 1;
				S4: if (delayCounter < 100) delayCounter <= delayCounter + 1;
					else				   delayCounter <= 0;
				S6: begin
					spiIn <= 0;
					readyForPixel <= 0;
				end
				S7: if (curPixel == lastPixel) runCount <= runCount + 1;
				S9: begin
					encodedByte <= {2'b11, runCountOut};
					if (finalPixel) jumpBack <= S18;
					else			jumpBack <= S1;
					runCount <= 0;
				end
				S11: begin
					encodedByte <= {2'b11, runCountOut};
					jumpBack <= S12;
					runCount <= 0;
				end
				S12: begin
					if (curPixel == prevSeen[index]) begin
						encodedByte <= {2'b00, index};
						if (finalPixel) jumpBack <= S18;
						else			jumpBack <= S1;
					end
					else prevSeen[index] <= curPixel;
				end
				S13: spiOut <= 1;
				S14: encodedByteReady <= 1;
				S15: if (delayCounter < 100) delayCounter <= delayCounter + 1;
					 else					delayCounter <= 0;
				S17: begin
					spiOut <= 0;
					encodedByteReady <= 0;
					if (jumpBack == S18) doneEncoding <= 1;
				end
				S18: if (jumpBack == S18) doneEncoding <= 1;
				S20: begin
					if (vr > -3 && vr < 2 && vg > -3 && vg < 2 && vb > -3 && vb < 2) begin
						encodedByte <= {2'b01, vrOut, vgOut, vbOut};
						if (finalPixel) jumpBack <= S18;
						else			jumpBack <= S1;
					end
				end
				S22: begin
					encodedByte <= 8'b11111111;
					jumpBack <= S23;
				end
				S23: begin
					encodedByte <= curPixelR;
					jumpBack <= S24;
				end
				S24: begin
					encodedByte <= curPixelG;
					jumpBack <= S25;
				end
				S25: begin
					encodedByte <= curPixelB;
					jumpBack <= S26;
				end
				S26: begin
					encodedByte <= curPixelA;
					if (finalPixel) jumpBack <= S18;
					else			jumpBack <= S1;
				end
				S27: begin
					encodedByte <= {2'b10, vgOut2};
					jumpBack <= S28;
				end
				S28: begin
					encodedByte <= {vg_rOut, vg_bOut};
					if (finalPixel) jumpBack <= S18;
					else			jumpBack <= S1;
				end
				S29: begin
					encodedByte <= 8'b11111110;
					jumpBack <= S30;
				end
				S30: begin
					encodedByte <= curPixelR;
					jumpBack <= S31;
				end
				S31: begin
					encodedByte <= curPixelG;
					jumpBack <= S32;
				end
				S32: begin
					encodedByte <= curPixelB;
					if (finalPixel) jumpBack <= S18;
					else			jumpBack <= S1;
				end
			endcase
endmodule