/* 
	Project:	DDS Function Generator
	File:		DDS_Core.v
	Date:		26/01/2018
	Autor:	Manuel

	Wrapper around the DDS logic
*/


module DDS_Core #(
	parameter 			_PHASE_WORD_WIDTH 		= 32,
	parameter 			_FIXED_POINT_EXP 			= 15,
	parameter signed	_FIXED_POINT_CONSTANT 	= 64'd1 << 15,
	parameter 			_FIXED_POITN_WIDTH 		= 16,
	parameter			_RAM_ADD_WIDTH				= 10,
	parameter			_RAM_DAT_WIDTH 			= 16,
	parameter 			_OUT_AMPLITUDE_WIDTH		= 12
	)(
	input													i_clk,
	input													i_reset,									//Resets all phase accumulators
	input				[_PHASE_WORD_WIDTH-1:0]		i_DesiredPhaseStep,					//Carrier frequency
	output			[_OUT_AMPLITUDE_WIDTH-1:0]	o_DDS_out,								//Waveform output to DAC
	//AM modulation
	input				[_PHASE_WORD_WIDTH-1:0]		i_AM_ModulationPhaseStep,			//AM modulation frequency
	input	signed	[_FIXED_POITN_WIDTH-1:0]	i_AM_ModulationIndex,				//AM modulation index (1 = 200%, 0.5 = 100%, 0 = 0%)
	//FM modulation
	input				[_PHASE_WORD_WIDTH-1:0]		i_FM_ModulationPhaseStep,			//FM modulation frequency
	input				[_PHASE_WORD_WIDTH-1:0]		i_FM_ModulationDeviationPhase,	//FM maximum frequency deviation
	//PM modulation
	input				[_PHASE_WORD_WIDTH-1:0]		i_PM_PhaseOffset,						//Phase Offset
	input				[_PHASE_WORD_WIDTH-1:0]		i_PM_ModulationPhaseStep,			//PM modulation frequancy
	input signed	[_FIXED_POITN_WIDTH-1:0]	i_PM_ModulationIndex,				//PM modulation index
	//RAM lookup tables
	output 												o_ram_isr,
	output			[_RAM_ADD_WIDTH-1:0]			o_ram_address,
	input  			[_RAM_DAT_WIDTH-1:0] 		i_ram_data,
	output 												o_ram_AM_isr,
	output   		[_RAM_ADD_WIDTH-1:0]  		o_ram_AM_address,
	input  			[_RAM_DAT_WIDTH-1:0] 		i_ram_AM_data,
	output 												o_ram_FM_isr,
	output   		[_RAM_ADD_WIDTH-1:0]  		o_ram_FM_address,
	input  			[_RAM_DAT_WIDTH-1:0] 		i_ram_FM_data
	);
	
	//Apply frequency modulation to the main waveform phase step 
	wire [_PHASE_WORD_WIDTH-1:0]	ModulatedPhaseStep;
	DDS_FM_Modulator #(	._PHASE_WORD_WIDTH		(_PHASE_WORD_WIDTH),
								._FIXED_POINT_EXP			(_FIXED_POINT_EXP),
								._FIXED_POINT_CONSTANT	(_FIXED_POINT_CONSTANT),
								._FIXED_POITN_WIDTH		(_FIXED_POITN_WIDTH),
								._RAM_ADD_WIDTH			(_RAM_ADD_WIDTH),
								._RAM_DAT_WIDTH			(_RAM_DAT_WIDTH)
								)
			FM_Mod (	.i_clk								(i_clk),
						.i_reset								(i_reset),
						.i_ModulationCenterPhaseStep	(i_DesiredPhaseStep),
						.i_ModulationPhaseStep			(i_FM_ModulationPhaseStep),
						.i_ModulationDeviationPhase	(i_FM_ModulationDeviationPhase),
						.o_ModulatedPhaseStep			(ModulatedPhaseStep),
						.o_ram_isr							(o_ram_FM_isr),
						.o_ram_address						(o_ram_FM_address),
						.i_ram_data							(i_ram_FM_data)
						);
	
	
	//Generate main waveform
	wire signed [_FIXED_POITN_WIDTH-1:0]	CarrierAmplitude;
	DDS_WaveGenerator #(	._PHASE_WORD_WIDTH		(_PHASE_WORD_WIDTH),
								._FIXED_POINT_EXP			(_FIXED_POINT_EXP),
								._FIXED_POINT_CONSTANT	(_FIXED_POINT_CONSTANT),
								._FIXED_POITN_WIDTH		(_FIXED_POITN_WIDTH),
								._RAM_ADD_WIDTH			(_RAM_ADD_WIDTH),
								._RAM_DAT_WIDTH			(_RAM_DAT_WIDTH)
								)
			DDSCarrier (	.i_clk			(i_clk),
								.i_reset			(i_reset),
								.i_PhaseStep	(ModulatedPhaseStep),
								.i_PhaseOffset	(i_PM_PhaseOffset),
								.o_Wave			(CarrierAmplitude),
								.o_ram_isr		(o_ram_isr),
								.o_ram_address	(o_ram_address),
								.i_ram_data		(i_ram_data)
							);
							
	
	//Apply amplitude modulation
	wire signed [_FIXED_POITN_WIDTH-1:0]	Amplitude;
	DDS_AM_Modulator #(	._PHASE_WORD_WIDTH		(_PHASE_WORD_WIDTH),
								._FIXED_POINT_EXP			(_FIXED_POINT_EXP),
								._FIXED_POINT_CONSTANT	(_FIXED_POINT_CONSTANT),
								._FIXED_POITN_WIDTH		(_FIXED_POITN_WIDTH),
								._RAM_ADD_WIDTH			(_RAM_ADD_WIDTH),
								._RAM_DAT_WIDTH			(_RAM_DAT_WIDTH)
								)
				AM_Mod (		.i_clk						(i_clk),
								.i_reset						(i_reset),
								.i_ModulationPhaseStep	(i_AM_ModulationPhaseStep),
								.i_ModulationIndex		(i_AM_ModulationIndex),
								.i_Carrier					(CarrierAmplitude),
								.o_AMOut						(Amplitude),
								.o_ram_isr					(o_ram_AM_isr),
								.o_ram_address				(o_ram_AM_address),
								.i_ram_data					(i_ram_AM_data)
							);
	
	
	//Amplitude truncation
	localparam AmplitudeDevider = 64'd1 << ((_FIXED_POINT_EXP - _OUT_AMPLITUDE_WIDTH) + 1);	//Costante per la quale divido l'ampiezza
	assign o_DDS_out = (Amplitude / AmplitudeDevider) + (64'd1 << (_OUT_AMPLITUDE_WIDTH - 1));
	
endmodule