/*
 * Author:    Kavi Dey, kdey@hmc.edu
 * Created:   12/3/2023
 * 
 * This file contains a single module with an inverted
 * 2:4 decoder
 */

 /*
 * module: decoder
 *
 * Decodes a 2 bit number into a an inverted select line
 *
 * Inputs:
 *    addr[1:0]: address
 *
 * Outputs:
 *    sel[3:0]: decoded selection
 */
module decoder(
      input   logic [1:0] addr,
      output  logic [3:0] sel
);
    logic [3:0] inverted_sel;

    assign sel = ~inverted_sel;

    always_comb begin
        case (addr)
            2'b00: inverted_sel = 4'b0001;
            2'b01: inverted_sel = 4'b0010;
            2'b10: inverted_sel = 4'b0100;
            2'b11: inverted_sel = 4'b1000;
        endcase
    end
endmodule