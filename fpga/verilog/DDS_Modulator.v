/* 
	Project:	DDS Function Generator
	File:		DDS_Modulator.v
	Date:		26/01/2018
	Autor:	Manuel
	
*/


//=======================================================
//  DDS_AM_Modulator
//
// Modulates in amplitude the input signal with a signal
// derived from the AM Loockup ram. Frequency and modulation depth
// are controlled by i_ModulationPhaseStep and i_ModulationIndex
//
// i_ModulationIndex = 1.0 * _FIXED_POINT_CONSTANT => 200%
// 						= 0.5 * _FIXED_POINT_CONSTANT => 100%
//							= 0.0 * _FIXED_POINT_CONSTANT => 0%
//
//=======================================================

module DDS_AM_Modulator #(
	parameter 			_PHASE_WORD_WIDTH 		= 32,
	parameter 			_FIXED_POINT_EXP 			= 15,
	parameter signed	_FIXED_POINT_CONSTANT	= 64'd1 << 15,
	parameter 			_FIXED_POITN_WIDTH 		= 16,
	parameter			_RAM_ADD_WIDTH				= 10,
	parameter			_RAM_DAT_WIDTH 			= 16
	)(
	input													i_clk,						//Clock
	input													i_reset,						//Reset: clear the phase accumulator
	input				[_PHASE_WORD_WIDTH-1:0]		i_ModulationPhaseStep,	//Modulation frequency
	input	 signed 	[_FIXED_POITN_WIDTH-1:0]	i_ModulationIndex,		//Modulation depth
	input	 signed	[_FIXED_POITN_WIDTH-1:0]	i_Carrier,					//Input signal to be modulated
	output signed	[_FIXED_POITN_WIDTH-1:0]	o_AMOut,						//Output signal
	output 												o_ram_isr,
	output			[_RAM_ADD_WIDTH-1:0]			o_ram_address,				//Lookup table ram
	input  			[_RAM_DAT_WIDTH-1:0] 		i_ram_data
	);
	
	//Generate modulation signal
	wire signed [_FIXED_POITN_WIDTH-1:0]	ModulationAmplitude;
	DDS_WaveGenerator #(	._PHASE_WORD_WIDTH(_PHASE_WORD_WIDTH),
								._FIXED_POINT_EXP(_FIXED_POINT_EXP),
								._FIXED_POINT_CONSTANT(_FIXED_POINT_CONSTANT),
								._FIXED_POITN_WIDTH(_FIXED_POITN_WIDTH),
								._RAM_ADD_WIDTH(_RAM_ADD_WIDTH),
								._RAM_DAT_WIDTH(_RAM_DAT_WIDTH)
								)
			Modulator (	.i_clk(i_clk),
							.i_reset(i_reset),
							.i_PhaseStep(i_ModulationPhaseStep),
							.i_PhaseOffset(0),
							.o_Wave(ModulationAmplitude),
							.o_ram_isr(o_ram_isr),
							.o_ram_address(o_ram_address),
							.i_ram_data(i_ram_data)
							);
	
	//Ammply modulation to the imput signal
	//A = signal amplitude, M = mod amplitude, m = mod frequency, c = signal frequency, I = modulation index
	//y(t) = [A + M*sen(f*t*m)] * sen(f*t*c) = [1-I + I*sen(f*t*m)] * sen(f*t*c)
	wire signed [_FIXED_POITN_WIDTH-1:0]	ModIndexOpposite = _FIXED_POINT_CONSTANT - i_ModulationIndex;
	wire signed [_FIXED_POITN_WIDTH-1:0]	ScaledModulation = (i_ModulationIndex * ModulationAmplitude) / _FIXED_POINT_CONSTANT;
	assign o_AMOut = ((ModIndexOpposite + ScaledModulation) * i_Carrier) / _FIXED_POINT_CONSTANT;
	
endmodule



//=======================================================
//  DDS_FM_Modulator
//
// Frequency modulation by shifting the phase increment.
// Modulation signal derived from the FM Loockup ram.
// Modulation frequency and frequancy deviation are
// controlled by i_ModulationPhaseStep and i_ModulationDeviationPhase
//
//=======================================================

module DDS_FM_Modulator #(
	parameter 			_PHASE_WORD_WIDTH 		= 32,
	parameter 			_FIXED_POINT_EXP 			= 15,
	parameter signed	_FIXED_POINT_CONSTANT	= 64'd1 << 15,
	parameter 			_FIXED_POITN_WIDTH 		= 16,
	parameter			_RAM_ADD_WIDTH				= 10,
	parameter			_RAM_DAT_WIDTH 			= 16
	)(
	input												i_clk,								//Clock
	input												i_reset,								//Reset: clear the phase acumulator
	input	signed	[_PHASE_WORD_WIDTH-1:0]	i_ModulationCenterPhaseStep,	//Center frequency to be modulated
	input				[_PHASE_WORD_WIDTH-1:0]	i_ModulationPhaseStep,			//Modulation frequency
	input signed 	[_PHASE_WORD_WIDTH-1:0]	i_ModulationDeviationPhase,	//Max frequncy deviation
	output 			[_PHASE_WORD_WIDTH-1:0]	o_ModulatedPhaseStep,			//Modulated output
	output 											o_ram_isr,
	output			[_RAM_ADD_WIDTH-1:0]  	o_ram_address,						//Lookup table ram
	input  			[_RAM_DAT_WIDTH-1:0] 	i_ram_data
	);
	
	//Generate modulation signal
	wire signed [_FIXED_POITN_WIDTH-1:0]	ModulationAmplitude;
	DDS_WaveGenerator #(	._PHASE_WORD_WIDTH(_PHASE_WORD_WIDTH),
								._FIXED_POINT_EXP(_FIXED_POINT_EXP),
								._FIXED_POINT_CONSTANT(_FIXED_POINT_CONSTANT),
								._FIXED_POITN_WIDTH(_FIXED_POITN_WIDTH),
								._RAM_ADD_WIDTH(_RAM_ADD_WIDTH),
								._RAM_DAT_WIDTH(_RAM_DAT_WIDTH)
								)
			Modulator (	.i_clk(i_clk),
							.i_reset(i_reset),
							.i_PhaseStep(i_ModulationPhaseStep),
							.i_PhaseOffset(0),
							.o_Wave(ModulationAmplitude),
							.o_ram_isr(o_ram_isr),
							.o_ram_address(o_ram_address),
							.i_ram_data(i_ram_data));
	
	//Compute output signal
	wire signed [_PHASE_WORD_WIDTH-1:0] frequencyDelta = (i_ModulationDeviationPhase * ModulationAmplitude) / _FIXED_POINT_CONSTANT;
	assign o_ModulatedPhaseStep = i_ModulationCenterPhaseStep + frequencyDelta;
endmodule
