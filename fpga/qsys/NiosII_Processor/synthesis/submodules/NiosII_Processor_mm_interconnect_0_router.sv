// (C) 2001-2018 Intel Corporation. All rights reserved.
// Your use of Intel Corporation's design tools, logic functions and other 
// software and tools, and its AMPP partner logic functions, and any output 
// files from any of the foregoing (including device programming or simulation 
// files), and any associated documentation or information are expressly subject 
// to the terms and conditions of the Intel Program License Subscription 
// Agreement, Intel FPGA IP License Agreement, or other applicable 
// license agreement, including, without limitation, that your use is for the 
// sole purpose of programming logic devices manufactured by Intel and sold by 
// Intel or its authorized distributors.  Please refer to the applicable 
// agreement for further details.



// Your use of Altera Corporation's design tools, logic functions and other 
// software and tools, and its AMPP partner logic functions, and any output 
// files any of the foregoing (including device programming or simulation 
// files), and any associated documentation or information are expressly subject 
// to the terms and conditions of the Altera Program License Subscription 
// Agreement, Altera MegaCore Function License Agreement, or other applicable 
// license agreement, including, without limitation, that your use is for the 
// sole purpose of programming logic devices manufactured by Altera and sold by 
// Altera or its authorized distributors.  Please refer to the applicable 
// agreement for further details.


// $Id: //acds/rel/18.1std/ip/merlin/altera_merlin_router/altera_merlin_router.sv.terp#1 $
// $Revision: #1 $
// $Date: 2018/07/18 $
// $Author: psgswbuild $

// -------------------------------------------------------
// Merlin Router
//
// Asserts the appropriate one-hot encoded channel based on 
// either (a) the address or (b) the dest id. The DECODER_TYPE
// parameter controls this behaviour. 0 means address decoder,
// 1 means dest id decoder.
//
// In the case of (a), it also sets the destination id.
// -------------------------------------------------------

`timescale 1 ns / 1 ns

module NiosII_Processor_mm_interconnect_0_router_default_decode
  #(
     parameter DEFAULT_CHANNEL = 35,
               DEFAULT_WR_CHANNEL = -1,
               DEFAULT_RD_CHANNEL = -1,
               DEFAULT_DESTID = 27 
   )
  (output [91 - 86 : 0] default_destination_id,
   output [47-1 : 0] default_wr_channel,
   output [47-1 : 0] default_rd_channel,
   output [47-1 : 0] default_src_channel
  );

  assign default_destination_id = 
    DEFAULT_DESTID[91 - 86 : 0];

  generate
    if (DEFAULT_CHANNEL == -1) begin : no_default_channel_assignment
      assign default_src_channel = '0;
    end
    else begin : default_channel_assignment
      assign default_src_channel = 47'b1 << DEFAULT_CHANNEL;
    end
  endgenerate

  generate
    if (DEFAULT_RD_CHANNEL == -1) begin : no_default_rw_channel_assignment
      assign default_wr_channel = '0;
      assign default_rd_channel = '0;
    end
    else begin : default_rw_channel_assignment
      assign default_wr_channel = 47'b1 << DEFAULT_WR_CHANNEL;
      assign default_rd_channel = 47'b1 << DEFAULT_RD_CHANNEL;
    end
  endgenerate

endmodule


module NiosII_Processor_mm_interconnect_0_router
(
    // -------------------
    // Clock & Reset
    // -------------------
    input clk,
    input reset,

    // -------------------
    // Command Sink (Input)
    // -------------------
    input                       sink_valid,
    input  [105-1 : 0]    sink_data,
    input                       sink_startofpacket,
    input                       sink_endofpacket,
    output                      sink_ready,

    // -------------------
    // Command Source (Output)
    // -------------------
    output                          src_valid,
    output reg [105-1    : 0] src_data,
    output reg [47-1 : 0] src_channel,
    output                          src_startofpacket,
    output                          src_endofpacket,
    input                           src_ready
);

    // -------------------------------------------------------
    // Local parameters and variables
    // -------------------------------------------------------
    localparam PKT_ADDR_H = 55;
    localparam PKT_ADDR_L = 36;
    localparam PKT_DEST_ID_H = 91;
    localparam PKT_DEST_ID_L = 86;
    localparam PKT_PROTECTION_H = 95;
    localparam PKT_PROTECTION_L = 93;
    localparam ST_DATA_W = 105;
    localparam ST_CHANNEL_W = 47;
    localparam DECODER_TYPE = 0;

    localparam PKT_TRANS_WRITE = 58;
    localparam PKT_TRANS_READ  = 59;

    localparam PKT_ADDR_W = PKT_ADDR_H-PKT_ADDR_L + 1;
    localparam PKT_DEST_ID_W = PKT_DEST_ID_H-PKT_DEST_ID_L + 1;



    // -------------------------------------------------------
    // Figure out the number of bits to mask off for each slave span
    // during address decoding
    // -------------------------------------------------------
    localparam PAD0 = log2ceil(64'h8000 - 64'h0); 
    localparam PAD1 = log2ceil(64'h8020 - 64'h8000); 
    localparam PAD2 = log2ceil(64'h8040 - 64'h8020); 
    localparam PAD3 = log2ceil(64'h8060 - 64'h8040); 
    localparam PAD4 = log2ceil(64'h8070 - 64'h8060); 
    localparam PAD5 = log2ceil(64'h8080 - 64'h8070); 
    localparam PAD6 = log2ceil(64'h8090 - 64'h8080); 
    localparam PAD7 = log2ceil(64'h80a0 - 64'h8090); 
    localparam PAD8 = log2ceil(64'h80b0 - 64'h80a0); 
    localparam PAD9 = log2ceil(64'h80c0 - 64'h80b0); 
    localparam PAD10 = log2ceil(64'h80d0 - 64'h80c0); 
    localparam PAD11 = log2ceil(64'h80e0 - 64'h80d0); 
    localparam PAD12 = log2ceil(64'h80f0 - 64'h80e0); 
    localparam PAD13 = log2ceil(64'h8100 - 64'h80f0); 
    localparam PAD14 = log2ceil(64'h8110 - 64'h8100); 
    localparam PAD15 = log2ceil(64'h8120 - 64'h8110); 
    localparam PAD16 = log2ceil(64'h8130 - 64'h8120); 
    localparam PAD17 = log2ceil(64'h8140 - 64'h8130); 
    localparam PAD18 = log2ceil(64'h8150 - 64'h8140); 
    localparam PAD19 = log2ceil(64'h8160 - 64'h8150); 
    localparam PAD20 = log2ceil(64'h8170 - 64'h8160); 
    localparam PAD21 = log2ceil(64'h8180 - 64'h8170); 
    localparam PAD22 = log2ceil(64'h8190 - 64'h8180); 
    localparam PAD23 = log2ceil(64'h81a0 - 64'h8190); 
    localparam PAD24 = log2ceil(64'h81b0 - 64'h81a0); 
    localparam PAD25 = log2ceil(64'h81c0 - 64'h81b0); 
    localparam PAD26 = log2ceil(64'h81d0 - 64'h81c0); 
    localparam PAD27 = log2ceil(64'h81e0 - 64'h81d0); 
    localparam PAD28 = log2ceil(64'h81f0 - 64'h81e0); 
    localparam PAD29 = log2ceil(64'h8200 - 64'h81f0); 
    localparam PAD30 = log2ceil(64'h8210 - 64'h8200); 
    localparam PAD31 = log2ceil(64'h8220 - 64'h8210); 
    localparam PAD32 = log2ceil(64'h9800 - 64'h9000); 
    localparam PAD33 = log2ceil(64'ha000 - 64'h9800); 
    localparam PAD34 = log2ceil(64'ha800 - 64'ha000); 
    localparam PAD35 = log2ceil(64'hb000 - 64'ha800); 
    localparam PAD36 = log2ceil(64'hb800 - 64'hb000); 
    localparam PAD37 = log2ceil(64'hc000 - 64'hb800); 
    localparam PAD38 = log2ceil(64'h80000 - 64'h40000); 
    localparam PAD39 = log2ceil(64'h89000 - 64'h88800); 
    localparam PAD40 = log2ceil(64'h89020 - 64'h89000); 
    localparam PAD41 = log2ceil(64'h89040 - 64'h89020); 
    localparam PAD42 = log2ceil(64'h89060 - 64'h89040); 
    localparam PAD43 = log2ceil(64'h89070 - 64'h89060); 
    localparam PAD44 = log2ceil(64'h89080 - 64'h89070); 
    localparam PAD45 = log2ceil(64'h89088 - 64'h89080); 
    localparam PAD46 = log2ceil(64'h89090 - 64'h89088); 
    // -------------------------------------------------------
    // Work out which address bits are significant based on the
    // address range of the slaves. If the required width is too
    // large or too small, we use the address field width instead.
    // -------------------------------------------------------
    localparam ADDR_RANGE = 64'h89090;
    localparam RANGE_ADDR_WIDTH = log2ceil(ADDR_RANGE);
    localparam OPTIMIZED_ADDR_H = (RANGE_ADDR_WIDTH > PKT_ADDR_W) ||
                                  (RANGE_ADDR_WIDTH == 0) ?
                                        PKT_ADDR_H :
                                        PKT_ADDR_L + RANGE_ADDR_WIDTH - 1;

    localparam RG = RANGE_ADDR_WIDTH-1;
    localparam REAL_ADDRESS_RANGE = OPTIMIZED_ADDR_H - PKT_ADDR_L;

      reg [PKT_ADDR_W-1 : 0] address;
      always @* begin
        address = {PKT_ADDR_W{1'b0}};
        address [REAL_ADDRESS_RANGE:0] = sink_data[OPTIMIZED_ADDR_H : PKT_ADDR_L];
      end   

    // -------------------------------------------------------
    // Pass almost everything through, untouched
    // -------------------------------------------------------
    assign sink_ready        = src_ready;
    assign src_valid         = sink_valid;
    assign src_startofpacket = sink_startofpacket;
    assign src_endofpacket   = sink_endofpacket;
    wire [PKT_DEST_ID_W-1:0] default_destid;
    wire [47-1 : 0] default_src_channel;






    NiosII_Processor_mm_interconnect_0_router_default_decode the_default_decode(
      .default_destination_id (default_destid),
      .default_wr_channel   (),
      .default_rd_channel   (),
      .default_src_channel  (default_src_channel)
    );

    always @* begin
        src_data    = sink_data;
        src_channel = default_src_channel;
        src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = default_destid;

        // --------------------------------------------------
        // Address Decoder
        // Sets the channel and destination ID based on the address
        // --------------------------------------------------

    // ( 0x0 .. 0x8000 )
    if ( {address[RG:PAD0],{PAD0{1'b0}}} == 20'h0   ) begin
            src_channel = 47'b00000000010000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 43;
    end

    // ( 0x8000 .. 0x8020 )
    if ( {address[RG:PAD1],{PAD1{1'b0}}} == 20'h8000   ) begin
            src_channel = 47'b10000000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 44;
    end

    // ( 0x8020 .. 0x8040 )
    if ( {address[RG:PAD2],{PAD2{1'b0}}} == 20'h8020   ) begin
            src_channel = 47'b00000000000001000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 45;
    end

    // ( 0x8040 .. 0x8060 )
    if ( {address[RG:PAD3],{PAD3{1'b0}}} == 20'h8040   ) begin
            src_channel = 47'b00000000000000100000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 32;
    end

    // ( 0x8060 .. 0x8070 )
    if ( {address[RG:PAD4],{PAD4{1'b0}}} == 20'h8060   ) begin
            src_channel = 47'b01000000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 34;
    end

    // ( 0x8070 .. 0x8080 )
    if ( {address[RG:PAD5],{PAD5{1'b0}}} == 20'h8070   ) begin
            src_channel = 47'b00100000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 25;
    end

    // ( 0x8080 .. 0x8090 )
    if ( {address[RG:PAD6],{PAD6{1'b0}}} == 20'h8080   ) begin
            src_channel = 47'b00010000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 0;
    end

    // ( 0x8090 .. 0x80a0 )
    if ( {address[RG:PAD7],{PAD7{1'b0}}} == 20'h8090   ) begin
            src_channel = 47'b00001000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 2;
    end

    // ( 0x80a0 .. 0x80b0 )
    if ( {address[RG:PAD8],{PAD8{1'b0}}} == 20'h80a0   ) begin
            src_channel = 47'b00000100000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 1;
    end

    // ( 0x80b0 .. 0x80c0 )
    if ( {address[RG:PAD9],{PAD9{1'b0}}} == 20'h80b0   ) begin
            src_channel = 47'b00000010000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 29;
    end

    // ( 0x80c0 .. 0x80d0 )
    if ( {address[RG:PAD10],{PAD10{1'b0}}} == 20'h80c0   ) begin
            src_channel = 47'b00000000000000000000001000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 24;
    end

    // ( 0x80d0 .. 0x80e0 )
    if ( {address[RG:PAD11],{PAD11{1'b0}}} == 20'h80d0   ) begin
            src_channel = 47'b00000000000000000000000100000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 13;
    end

    // ( 0x80e0 .. 0x80f0 )
    if ( {address[RG:PAD12],{PAD12{1'b0}}} == 20'h80e0   ) begin
            src_channel = 47'b00000000000000000000000010000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 23;
    end

    // ( 0x80f0 .. 0x8100 )
    if ( {address[RG:PAD13],{PAD13{1'b0}}} == 20'h80f0   ) begin
            src_channel = 47'b00000000000000000000000001000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 12;
    end

    // ( 0x8100 .. 0x8110 )
    if ( {address[RG:PAD14],{PAD14{1'b0}}} == 20'h8100   ) begin
            src_channel = 47'b00000000000000000000000000100000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 22;
    end

    // ( 0x8110 .. 0x8120 )
    if ( {address[RG:PAD15],{PAD15{1'b0}}} == 20'h8110   ) begin
            src_channel = 47'b00000000000000000000000000010000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 11;
    end

    // ( 0x8120 .. 0x8130 )
    if ( {address[RG:PAD16],{PAD16{1'b0}}} == 20'h8120   ) begin
            src_channel = 47'b00000000000000000000000000001000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 21;
    end

    // ( 0x8130 .. 0x8140 )
    if ( {address[RG:PAD17],{PAD17{1'b0}}} == 20'h8130   ) begin
            src_channel = 47'b00000000000000000000000000000100000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 10;
    end

    // ( 0x8140 .. 0x8150 )
    if ( {address[RG:PAD18],{PAD18{1'b0}}} == 20'h8140   ) begin
            src_channel = 47'b00000000000000000000000000000010000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 20;
    end

    // ( 0x8150 .. 0x8160 )
    if ( {address[RG:PAD19],{PAD19{1'b0}}} == 20'h8150   ) begin
            src_channel = 47'b00000000000000000000000000000001000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 9;
    end

    // ( 0x8160 .. 0x8170 )
    if ( {address[RG:PAD20],{PAD20{1'b0}}} == 20'h8160   ) begin
            src_channel = 47'b00000000000000000000000000000000100000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 19;
    end

    // ( 0x8170 .. 0x8180 )
    if ( {address[RG:PAD21],{PAD21{1'b0}}} == 20'h8170   ) begin
            src_channel = 47'b00000000000000000000000000000000010000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 8;
    end

    // ( 0x8180 .. 0x8190 )
    if ( {address[RG:PAD22],{PAD22{1'b0}}} == 20'h8180   ) begin
            src_channel = 47'b00000000000000000000000000000000001000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 18;
    end

    // ( 0x8190 .. 0x81a0 )
    if ( {address[RG:PAD23],{PAD23{1'b0}}} == 20'h8190   ) begin
            src_channel = 47'b00000000000000000000000000000000000100000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 7;
    end

    // ( 0x81a0 .. 0x81b0 )
    if ( {address[RG:PAD24],{PAD24{1'b0}}} == 20'h81a0   ) begin
            src_channel = 47'b00000000000000000000000000000000000010000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 17;
    end

    // ( 0x81b0 .. 0x81c0 )
    if ( {address[RG:PAD25],{PAD25{1'b0}}} == 20'h81b0   ) begin
            src_channel = 47'b00000000000000000000000000000000000001000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 6;
    end

    // ( 0x81c0 .. 0x81d0 )
    if ( {address[RG:PAD26],{PAD26{1'b0}}} == 20'h81c0   ) begin
            src_channel = 47'b00000000000000000000000000000000000000100000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 16;
    end

    // ( 0x81d0 .. 0x81e0 )
    if ( {address[RG:PAD27],{PAD27{1'b0}}} == 20'h81d0   ) begin
            src_channel = 47'b00000000000000000000000000000000000000010000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 5;
    end

    // ( 0x81e0 .. 0x81f0 )
    if ( {address[RG:PAD28],{PAD28{1'b0}}} == 20'h81e0   ) begin
            src_channel = 47'b00000000000000000000000000000000000000000001000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 15;
    end

    // ( 0x81f0 .. 0x8200 )
    if ( {address[RG:PAD29],{PAD29{1'b0}}} == 20'h81f0   ) begin
            src_channel = 47'b00000000000000000000000000000000000000000000100;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 4;
    end

    // ( 0x8200 .. 0x8210 )
    if ( {address[RG:PAD30],{PAD30{1'b0}}} == 20'h8200   ) begin
            src_channel = 47'b00000000000000000000000000000000000000000000010;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 14;
    end

    // ( 0x8210 .. 0x8220 )
    if ( {address[RG:PAD31],{PAD31{1'b0}}} == 20'h8210   ) begin
            src_channel = 47'b00000000000000000000000000000000000000000000001;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 3;
    end

    // ( 0x9000 .. 0x9800 )
    if ( {address[RG:PAD32],{PAD32{1'b0}}} == 20'h9000   ) begin
            src_channel = 47'b00000000000000000000010000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 35;
    end

    // ( 0x9800 .. 0xa000 )
    if ( {address[RG:PAD33],{PAD33{1'b0}}} == 20'h9800   ) begin
            src_channel = 47'b00000000000000000000100000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 36;
    end

    // ( 0xa000 .. 0xa800 )
    if ( {address[RG:PAD34],{PAD34{1'b0}}} == 20'ha000   ) begin
            src_channel = 47'b00000000000000000001000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 37;
    end

    // ( 0xa800 .. 0xb000 )
    if ( {address[RG:PAD35],{PAD35{1'b0}}} == 20'ha800   ) begin
            src_channel = 47'b00000000000000000010000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 38;
    end

    // ( 0xb000 .. 0xb800 )
    if ( {address[RG:PAD36],{PAD36{1'b0}}} == 20'hb000   ) begin
            src_channel = 47'b00000000000000000100000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 39;
    end

    // ( 0xb800 .. 0xc000 )
    if ( {address[RG:PAD37],{PAD37{1'b0}}} == 20'hb800   ) begin
            src_channel = 47'b00000000000000001000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 40;
    end

    // ( 0x40000 .. 0x80000 )
    if ( {address[RG:PAD38],{PAD38{1'b0}}} == 20'h40000   ) begin
            src_channel = 47'b00000000000100000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 27;
    end

    // ( 0x88800 .. 0x89000 )
    if ( {address[RG:PAD39],{PAD39{1'b0}}} == 20'h88800   ) begin
            src_channel = 47'b00000000001000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 41;
    end

    // ( 0x89000 .. 0x89020 )
    if ( {address[RG:PAD40],{PAD40{1'b0}}} == 20'h89000   ) begin
            src_channel = 47'b00000001000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 46;
    end

    // ( 0x89020 .. 0x89040 )
    if ( {address[RG:PAD41],{PAD41{1'b0}}} == 20'h89020   ) begin
            src_channel = 47'b00000000100000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 42;
    end

    // ( 0x89040 .. 0x89060 )
    if ( {address[RG:PAD42],{PAD42{1'b0}}} == 20'h89040   ) begin
            src_channel = 47'b00000000000000000000000000000000000000000100000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 31;
    end

    // ( 0x89060 .. 0x89070 )
    if ( {address[RG:PAD43],{PAD43{1'b0}}} == 20'h89060   ) begin
            src_channel = 47'b00000000000000000000000000000000000000001000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 33;
    end

    // ( 0x89070 .. 0x89080 )
    if ( {address[RG:PAD44],{PAD44{1'b0}}} == 20'h89070   ) begin
            src_channel = 47'b00000000000000000000000000000000000000000010000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 30;
    end

    // ( 0x89080 .. 0x89088 )
    if ( {address[RG:PAD45],{PAD45{1'b0}}} == 20'h89080   ) begin
            src_channel = 47'b00000000000010000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 26;
    end

    // ( 0x89088 .. 0x89090 )
    if ( {address[RG:PAD46],{PAD46{1'b0}}} == 20'h89088   ) begin
            src_channel = 47'b00000000000000010000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 28;
    end

end


    // --------------------------------------------------
    // Ceil(log2()) function
    // --------------------------------------------------
    function integer log2ceil;
        input reg[65:0] val;
        reg [65:0] i;

        begin
            i = 1;
            log2ceil = 0;

            while (i < val) begin
                log2ceil = log2ceil + 1;
                i = i << 1;
            end
        end
    endfunction

endmodule


