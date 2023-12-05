/*
 * Authors:    Kavi Dey, kdey@hmc.edu
 *             Neil Chulani, nchulani@.hmc.edu
 * Created:   12/3/2023
 * 
 * This file contains a single module that multiplexes two
 * 4 bit binary numbers
 */

/*
 * module: neo_obscura_top
 *
 * Decodes a 2 bit number into a an inverted select line
 *
 * Inputs:
 *    addr[1:0]: address
 *
 * Outputs:
 *    sel[3:0]: decoded selection
 */
module neo_obscura_top (
    input logic sck,
    input logic copi,
    input logic reset,
    input logic [1:0] addr,
    output logic cipo,
    output logic [3:0] sel
);
  decoder address_decoder (
      addr,
      sel
  );
  qoi2top qoi (
      sck,
      copi,
      reset,
      cipo
  );
endmodule
