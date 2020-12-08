/* 
	Project:	DDS Function Generator
	File:		DDS_WaveGenerator.v
	Date:		26/01/2018
	Autor:	Manuel
	
*/

//=======================================================
//  DDS_WaveGenerator
//
// Takes data from the lookup ram and generates output waveform
//
//=======================================================

module DDS_WaveGenerator#(
	parameter 			_PHASE_WORD_WIDTH = 32,
	parameter	 		_FIXED_POINT_EXP = 15,
	parameter signed	_FIXED_POINT_CONSTANT = 64'd1 << 15,
	parameter 			_FIXED_POITN_WIDTH = 16,
	parameter			_RAM_ADD_WIDTH	= 10,
	parameter			_RAM_DAT_WIDTH = 16
	)(
	input													i_clk,			//Clock
	input													i_reset,			//Reset: clears phase accumulator
	input				[_PHASE_WORD_WIDTH -1:0]	i_PhaseStep,	//Value by which the phase accumulator is incremented each clock cycle
	input				[_PHASE_WORD_WIDTH -1:0]	i_PhaseOffset,	//Phase offset addad to the signal
	output signed	[_FIXED_POITN_WIDTH-1:0]	o_Wave,			//Signal output
	
	output 								o_ram_isr,
	output	[_RAM_ADD_WIDTH-1:0]	o_ram_address,					//Lookup table ram
	input  	[_RAM_DAT_WIDTH-1:0] i_ram_data
	);
	
	
	//Phase register instantiation
	wire [_PHASE_WORD_WIDTH-1:0]	CurrentPhase;					//Signal phase
	DDS_PhaseReg #(._PHASE_WORD_WIDTH(_PHASE_WORD_WIDTH))
					phaseReg(	.i_clk(i_clk),
									.i_reset(i_reset),
									.i_PhaseStep(i_PhaseStep),
									.o_CurrentPhase(CurrentPhase));
	
	//Add phase offset
	reg 	[_PHASE_WORD_WIDTH-1:0]	CurrentPhaseOfsetted = 0;
	always @(posedge i_clk) begin
		CurrentPhaseOfsetted <= CurrentPhase + i_PhaseOffset;
	end
	
	//Phase to amplitude conversion
	wire signed [_FIXED_POITN_WIDTH-1:0]	Amplitude;
	DDS_PhaseToAmplitude #(	._PHASE_WORD_WIDTH(_PHASE_WORD_WIDTH),					
									._FIXED_POINT_EXP(_FIXED_POINT_EXP),
									._FIXED_POINT_CONSTANT(_FIXED_POINT_CONSTANT),
									._FIXED_POITN_WIDTH(_FIXED_POITN_WIDTH)
									)
					phaseToA(	.i_clk(i_clk),
									.i_reset(i_reset),
									.i_phase(CurrentPhaseOfsetted),
									.o_amplitude(Amplitude),
									.o_ram_isr(o_ram_isr),
									.o_ram_address(o_ram_address),
									.i_ram_data(i_ram_data));
	
	//Amplitude
	assign o_Wave = Amplitude;
	
endmodule



//=======================================================
//  DDS_PhaseReg
//
// Phase register.
// Increments by i_PhaseStep on every clock cycle
// 
//=======================================================

module DDS_PhaseReg #(
	parameter _PHASE_WORD_WIDTH = 32
	)(
	input 										i_clk,			//Clock
	input											i_reset,			//Reset: clears phase register
	input		[_PHASE_WORD_WIDTH - 1:0]	i_PhaseStep,	//Value by which the phase accumulator is incremented each clock cycle
	output	[_PHASE_WORD_WIDTH - 1:0]	o_CurrentPhase	//Output phase
	);
	
	wire	[_PHASE_WORD_WIDTH - 1:0]	NextPhaseRegister;
	reg 	[_PHASE_WORD_WIDTH - 1:0]	PhaseRegister = 0;
	assign o_CurrentPhase = PhaseRegister;
	
	assign NextPhaseRegister = PhaseRegister + i_PhaseStep;
	
	always @(posedge i_clk) begin
		if(i_reset)											//If reset is asserted
			PhaseRegister <= 0;							//Clear phase register
		else													//Else
			PhaseRegister <= NextPhaseRegister;		//Update phase register
	end
	
endmodule


//=======================================================
//  DDS_PhaseToAmplitude
//
// Compute signal amplitude from the current phase and lookup RAM data
//
//=======================================================

module DDS_PhaseToAmplitude #(
	parameter 			_PHASE_WORD_WIDTH 		= 32,
	parameter 			_FIXED_POINT_EXP 			= 15,
	parameter signed	_FIXED_POINT_CONSTANT	= 64'd1 << 15,
	parameter 			_FIXED_POITN_WIDTH 		= 16,
	
	parameter			_RAM_ADD_WIDTH	= 10,					//Lookup RAM address bus widh
	parameter			_RAM_DAT_WIDTH = 16					//Lookup RAM data bus widh
	)(
	input											i_clk,			//Clock
	input											i_reset,			//Reset
	input		[_PHASE_WORD_WIDTH - 1:0]	i_phase,			//Current phase
	output	[_FIXED_POITN_WIDTH-1:0]	o_amplitude,	//Output amplitude
	
	output 										o_ram_isr,		// Signal sincronized with the i_phase (LOW first half ram, HIGH second half ram)
	output 			[_RAM_ADD_WIDTH-1:0]	o_ram_address,	//Lookup RAM address bus
	input signed 	[_RAM_DAT_WIDTH-1:0]	i_ram_data 		//Lookup RAM data bus
	);
	
	assign o_ram_isr 		= i_phase[_PHASE_WORD_WIDTH-1];							//ISR on phase MSB
	assign o_ram_address	= i_phase[_PHASE_WORD_WIDTH-1 -:_RAM_ADD_WIDTH];	//Address = phase word top bits
	assign o_amplitude	= i_ram_data;													//Output RAM data
	
endmodule