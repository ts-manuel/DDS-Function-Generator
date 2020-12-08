
//=======================================================
//  DDS_Sine_LUT
//
//	Modulo che traduce la fase nell'ampiezza di una sinusoide
//=======================================================

module SineLUT #(
	parameter _PHASE_WORD_WIDTH = 32,					//Numero di bit usati per rappresentare la fase
	parameter _FIXED_POINT_EXP = 16,						//Esponente della costante _FIXED_POINT_CONSTANT = (2 ^ _FIXED_POINT_EXP)
	parameter _FIXED_POINT_CONSTANT = 32'd1 << 16,	//Costante per la quale sono moltiplicati i numeri a virgola fissa
	parameter _FIXED_POITN_WIDTH = 24,					//Numero di bit usati per rappresentare un numero a virgola fissa
	parameter _PHASE_TRUNCATION = 12						//Numero di bit usati per indirizzare la LUT
	)(
	input													i_clk,
	input				[_PHASE_WORD_WIDTH-1:0]		i_phase,		//Fase del segnale di cui calcolare l'ampiezza
	output signed	[_FIXED_POITN_WIDTH-1:0]	o_amplitude	//Ampiezza del segnale 1 -1 in vircola fissa (*2^_FIXED_POINT_EXP)
	);
	
	
	localparam MaxPhase = 64'd1 << _PHASE_TRUNCATION;	//Valore massimo che la fase può assumere
	localparam Phase90  = MaxPhase / 4;
	localparam Phase180 = MaxPhase / 2;
	localparam Phase270 = Phase180 + Phase90;
	localparam LUTSize  = MaxPhase / 4;
	
	//reg signed	[_FIXED_POITN_WIDTH-1:0]	LUT[LUTSize:0];	//Registro che conterra i campioni della sinusoide
	///initial $readmemh("SineLUT.txt", LUT);							//Carico il registro con i campioni
	
	//Fase del segnale ridotta in numero di bit
	wire [_PHASE_TRUNCATION-1:0] sinePhase = i_phase[_PHASE_WORD_WIDTH-1:_PHASE_WORD_WIDTH - _PHASE_TRUNCATION];
	
	//Registro che conterra l'ampiezza della sinusoide alla fase corrente
	reg [_FIXED_POITN_WIDTH-1:0] amplitude = 0;
	assign o_amplitude = amplitude;
	
	reg	[9:0]		lut_address;
	wire	  			lut_clock;
	wire [11:0]  	lut_q;
	assign lut_q = lut_address;
	/*ROM_LUT lut0(	.address(lut_address),
						.clock(lut_clock),
						.q(lut_q));*/
	
	//always @(i_phase) begin
	//assign o_amplitude 	= lut_q << 4;
	//assign lut_address 	= sinePhase;
	assign lut_clock = i_clk;
	always @* begin
		if(sinePhase < Phase90) begin				//da 0° a 90°
			//amplitude = LUT[sinePhase];
			lut_address	= sinePhase;
			amplitude 	= lut_q << 4;
		end
		else if (sinePhase < Phase180)  begin	//da 90° a 180°
			//amplitude = LUT[Phase90 - (sinePhase - Phase90)];
			lut_address	= (Phase90 - 1) - sinePhase;
			amplitude = lut_q << 4;
			//amplitude = 2;
		end
		else if (sinePhase < Phase270) begin	//da 180° a 270°
			//amplitude = 0 - LUT[sinePhase - Phase180];
			lut_address	= sinePhase - Phase180;
			amplitude = 0 - lut_q << 4;
			//amplitude = 2;
		end
		else begin									//da 270° a 0°
			//amplitude = 0 - LUT[Phase90 - (sinePhase - Phase270)];
			lut_address	= (Phase90 - 1) - (sinePhase - Phase270);
			amplitude = 0 - lut_q << 4;
			//amplitude = 2;
		end
	end
	
	//Logica per riprodurre la sinusoide da campioni di un solo quarto di periodo
	/*always @* begin
		/*if(sinePhase < Phase90) begin				//da 0° a 90°
			amplitude = LUT[sinePhase];
		end
		else if (sinePhase < Phase180)  begin	//da 90° a 180°
			amplitude = LUT[Phase90 - (sinePhase - Phase90)];
		end
		else if (sinePhase < Phase270) begin	//da 180° a 270°
			amplitude = 0 - LUT[sinePhase - Phase180];
		end
		else begin									//da 270° a 0°
			amplitude = 0 - LUT[Phase90 - (sinePhase - Phase270)];
		end
	end*/

endmodule