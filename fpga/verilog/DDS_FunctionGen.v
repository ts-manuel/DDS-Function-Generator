/* 
	Project:	DDS Function Generator
	File:		DDS_FunctionGen.v
	Date:		26/01/2018
	Autor:	Manuel
	
	Top level module
*/

module DDS_FunctionGen (
	input  wire			i_clk_10Mhz, i_extclk,	//Clock inputs
	output wire			o_LED_debug,				//Debug led output
	//LCD
	output wire			o_LCD_BackLight_PWM,		//LCD BakLight intensity
	output wire	[4:0] o_LCD_Control,				//LCD Control Signals: E, RW, A0, RST, CS
	output wire	[7:0]	o_LCD_Data,					//LCD Bi-directional, three-state data bus lines
	//KEYPAD
	input	 wire	[3:0]	i_KeypadRows,				//Output to keypad rows
	output wire	[3:0]	o_keypadColumns,			//Input from keypad columns
	//BUTTONS AND ENCODER
	input	 wire	[5:0]	i_btnDisplay,
	input	 wire	[4:0] i_Encoder,
	//BUTTONS CH ON_OFF
	input	 wire			i_btn_CH0_ONOFF,
	input	 wire			i_btn_CH1_ONOFF,
	//SD CARD
	input	 wire			sd_spi_MISO,
	output wire			sd_spi_MOSI,
	output wire			sd_spi_SCLK,
	output wire			sd_spi_SS_n,
	//GPIO
	output wire	[9:0]	o_GPIO,
	 
	//DDS0
	output wire			o_DDS0_Relay, 
	output wire			o_LED_CH0,
	//output 			o_DDS0_Synk,
	output wire			o_DDS0_Gain,	//PWM output Gain
	output wire			o_DDS0_Offset,	//PWM output Offset
	output wire [11:0]o_DDS0_DAC, 	//12bit DAC
	//DDS1
	output wire			o_DDS1_Relay,
	output wire			o_LED_CH1,
	//output 			o_DDS1_Synk,
	output wire			o_DDS1_Gain,	//PWM output Gain
	output wire			o_DDS1_Offset,	//PWM output Offset
	output wire [11:0]o_DDS1_DAC		//12bit DAC
	);
	
	parameter 			_PHASE_WORD_WIDTH 		= 32;
	parameter 			_FIXED_POINT_EXP 			= 15;
	parameter signed	_FIXED_POINT_CONSTANT	= 64'd1 << 15;
	parameter 			_FIXED_POITN_WIDTH 		= 16;
	parameter 			_OUT_AMPLITUDE_WIDTH 	= 12;
	parameter			_RAM_ADD_WIDTH				= 10;
	parameter			_RAM_DAT_WIDTH 			= 16;
	
	wire cpuReset;
	
	//Clock
	wire	clk_200Mhz;						//200Mhz DDS Clock
	wire	clk_100Mhz;						//100Mhz CPU Clock
	wire	clk_2Khz;						//2Khz keypad Clock
	wire	EnableExternalClock = 0;	//0 = use internal clock, 1 = use external clock
	wire	ExternalClockBad;				//0 = external clock present, 1 = external clock unavailable
	//assign EnableExternalClock = !ExternalClockBad;	//Use externa clock when present
	
	PLL0 PLL0(	//.clkswitch	(EnableExternalClock),
					.inclk0		(i_clk_10Mhz),
					//.inclk1		(i_extclk),
					.c0			(clk_200Mhz),
					.c1			(clk_100Mhz),
					.c2			(clk_2Khz)/*,
					.clkbad1		(ExternalClockBad)*/);
					
	//DD0 Amplitude PWM
	wire [11:0] PMW_DDS0_Amplitude_DutyCycle;
	PWM #(._NUM_BIT(12))	pwm_DDS0_Amplitude (	.i_clk		(clk_200Mhz),
															.i_dutyCycle(PMW_DDS0_Amplitude_DutyCycle),
															.o_PWM		(o_DDS0_Gain));
	//DDS0 Offset PWM
	wire [11:0] PMW_DDS0_Offset_DutyCycle;		
	PWM #(._NUM_BIT(12))	pwm_DDS0_Offset (	.i_clk			(clk_200Mhz),
														.i_dutyCycle	(PMW_DDS0_Offset_DutyCycle),
														.o_PWM			(o_DDS0_Offset));
	//DDS1 Amplitude PWM
	wire [11:0] PMW_DDS1_Amplitude_DutyCycle;
	PWM #(._NUM_BIT(12))	pwm_DDS1_Amplitude (	.i_clk		(clk_200Mhz),
															.i_dutyCycle(PMW_DDS1_Amplitude_DutyCycle),
															.o_PWM		(o_DDS1_Gain));
	//DDS1 Offset PWM
	wire [11:0] PMW_DDS1_Offset_DutyCycle;
	PWM #(._NUM_BIT(12))	pwm_DDS1_Offset (	.i_clk			(clk_200Mhz),
														.i_dutyCycle	(PMW_DDS1_Offset_DutyCycle),
														.o_PWM			(o_DDS1_Offset));
	//LCD BackLight PWM
	wire [7:0] PMW_LCD_BackLight_DutyCycle;
	PWM #(._NUM_BIT(8))	pwm_LCD_BackLight (	.i_clk		(clk_100Mhz),
															.i_dutyCycle(PMW_LCD_BackLight_DutyCycle),
															.o_PWM		(o_LCD_BackLight_PWM));

	
	wire [5:0] lookup_ram_isr;
	
	//DDS_0 wires to CPU
	wire												DDS0_Reset;
	wire 			[_PHASE_WORD_WIDTH-1:0]		DDS0_DesiredPhaseStep;
	wire			[_PHASE_WORD_WIDTH-1:0]		DDS0_AM_ModulationPhaseStep;
	wire signed	[_FIXED_POITN_WIDTH-1:0]	DDS0_AM_ModulationIndex;
	wire			[_PHASE_WORD_WIDTH-1:0]		DDS0_FM_ModulationPhaseStep;
	wire			[_PHASE_WORD_WIDTH-1:0]		DDS0_FM_ModulationDeviationPhase;
	wire			[_PHASE_WORD_WIDTH-1:0]		DDS0_PM_PhaseOffset;
	wire			[_PHASE_WORD_WIDTH-1:0]		DDS0_PM_ModulationPhaseStep;
	wire signed	[_FIXED_POITN_WIDTH-1:0]	DDS0_PM_ModulationIndex;

	wire [_RAM_ADD_WIDTH-1:0]	ram_dds0_address;
	wire [_RAM_DAT_WIDTH-1:0]	ram_dds0_readdata;
	wire [_RAM_ADD_WIDTH-1:0]	ram_dds0_AM_address;
	wire [_RAM_DAT_WIDTH-1:0]	ram_dds0_AM_readdata;
	wire [_RAM_ADD_WIDTH-1:0]	ram_dds0_FM_address;
	wire [_RAM_DAT_WIDTH-1:0]	ram_dds0_FM_readdata;
	
	DDS_Core #(	._PHASE_WORD_WIDTH		(_PHASE_WORD_WIDTH),
					._FIXED_POINT_EXP			(_FIXED_POINT_EXP),
					._FIXED_POINT_CONSTANT	(_FIXED_POINT_CONSTANT),
					._FIXED_POITN_WIDTH		(_FIXED_POITN_WIDTH),
					._RAM_ADD_WIDTH			(_RAM_ADD_WIDTH),
					._RAM_DAT_WIDTH			(_RAM_DAT_WIDTH),
					._OUT_AMPLITUDE_WIDTH	(_OUT_AMPLITUDE_WIDTH))
				dds0(	.i_clk					(clk_200Mhz),
						.i_reset					(DDS0_Reset),
						.i_DesiredPhaseStep	(DDS0_DesiredPhaseStep),
						
						.i_AM_ModulationPhaseStep	(DDS0_AM_ModulationPhaseStep),
						.i_AM_ModulationIndex		(DDS0_AM_ModulationIndex),
						
						.i_FM_ModulationPhaseStep			(DDS0_FM_ModulationPhaseStep),
						.i_FM_ModulationDeviationPhase	(DDS0_FM_ModulationDeviationPhase),
						
						.i_PM_PhaseOffset				(DDS0_PM_PhaseOffset),
						.i_PM_ModulationPhaseStep	(DDS0_PM_ModulationPhaseStep),
						.i_PM_ModulationIndex		(DDS0_PM_ModulationIndex),
						
						.o_ram_isr			(lookup_ram_isr[0]),
						.o_ram_address		(ram_dds0_address),
						.i_ram_data			(ram_dds0_readdata),
						.o_ram_AM_isr		(lookup_ram_isr[1]),
						.o_ram_AM_address	(ram_dds0_AM_address),
						.i_ram_AM_data		(ram_dds0_AM_readdata),
						.o_ram_FM_isr		(lookup_ram_isr[2]),
						.o_ram_FM_address	(ram_dds0_FM_address),
						.i_ram_FM_data		(ram_dds0_FM_readdata),
					 
						.o_DDS_out	(o_DDS0_DAC)
						);
	
	//DDS_1 wires to CPU
	wire												DDS1_Reset;
	wire 			[_PHASE_WORD_WIDTH-1:0]		DDS1_DesiredPhaseStep;
	wire			[_PHASE_WORD_WIDTH-1:0]		DDS1_AM_ModulationPhaseStep;
	wire signed	[_FIXED_POITN_WIDTH-1:0]	DDS1_AM_ModulationIndex;
	wire			[_PHASE_WORD_WIDTH-1:0]		DDS1_FM_ModulationPhaseStep;
	wire			[_PHASE_WORD_WIDTH-1:0]		DDS1_FM_ModulationDeviationPhase;
	wire			[_PHASE_WORD_WIDTH-1:0]		DDS1_PM_PhaseOffset;
	wire			[_PHASE_WORD_WIDTH-1:0]		DDS1_PM_ModulationPhaseStep;
	wire signed	[_FIXED_POITN_WIDTH-1:0]	DDS1_PM_ModulationIndex;
	
	wire [_RAM_ADD_WIDTH-1:0]	ram_dds1_address;
	wire [_RAM_DAT_WIDTH-1:0]	ram_dds1_readdata;
	wire [_RAM_ADD_WIDTH-1:0]	ram_dds1_AM_address;
	wire [_RAM_DAT_WIDTH-1:0]	ram_dds1_AM_readdata;
	wire [_RAM_ADD_WIDTH-1:0]	ram_dds1_FM_address;
	wire [_RAM_DAT_WIDTH-1:0]	ram_dds1_FM_readdata;
	
	DDS_Core #(	._PHASE_WORD_WIDTH		(_PHASE_WORD_WIDTH),
					._FIXED_POINT_EXP			(_FIXED_POINT_EXP),
					._FIXED_POINT_CONSTANT	(_FIXED_POINT_CONSTANT),
					._FIXED_POITN_WIDTH		(_FIXED_POITN_WIDTH),
					._RAM_ADD_WIDTH			(_RAM_ADD_WIDTH),
					._RAM_DAT_WIDTH			(_RAM_DAT_WIDTH),
					._OUT_AMPLITUDE_WIDTH	(_OUT_AMPLITUDE_WIDTH))
				dds1(	.i_clk					(clk_200Mhz),
						.i_reset					(DDS1_Reset),
						.i_DesiredPhaseStep	(DDS1_DesiredPhaseStep),
						
						.i_AM_ModulationPhaseStep	(DDS1_AM_ModulationPhaseStep),
						.i_AM_ModulationIndex		(DDS1_AM_ModulationIndex),
						
						.i_FM_ModulationPhaseStep			(DDS1_FM_ModulationPhaseStep),
						.i_FM_ModulationDeviationPhase	(DDS1_FM_ModulationDeviationPhase),
						
						.i_PM_PhaseOffset				(DDS1_PM_PhaseOffset),
						.i_PM_ModulationPhaseStep	(DDS1_PM_ModulationPhaseStep),
						.i_PM_ModulationIndex		(DDS1_PM_ModulationIndex),
						
						.o_ram_isr			(lookup_ram_isr[3]),
						.o_ram_address		(ram_dds1_address),
						.i_ram_data			(ram_dds1_readdata),
						.o_ram_AM_isr		(lookup_ram_isr[4]),
						.o_ram_AM_address	(ram_dds1_AM_address),
						.i_ram_AM_data		(ram_dds1_AM_readdata),
						.o_ram_FM_isr		(lookup_ram_isr[5]),
						.o_ram_FM_address	(ram_dds1_FM_address),
						.i_ram_FM_data		(ram_dds1_FM_readdata),
						
						.o_DDS_out	(o_DDS1_DAC)
						);
	
	//Output reay
	wire Relay_DDS0;
	assign o_DDS0_Relay	= Relay_DDS0;
	assign o_LED_CH0 		= Relay_DDS0;
	wire Relay_DDS1;
	assign o_DDS1_Relay 	= Relay_DDS1;
	assign o_LED_CH1 		= Relay_DDS1;
	
	//LCD Control signals
	wire [4:0]  lcd_dma_address;		//DMA write address
	wire        lcd_dma_chipselect;	//DMA chipselect
	wire        lcd_dma_waitrequest;	//DMA waitrequest
	wire        lcd_dma_write_n;		//DMA write_n
	wire [7:0]  lcd_dma_writedata; 	//DMA data
	wire [4:0] 	LCD_Control_CPU;
	wire [7:0] 	LCD_Data_CPU;
   
	LCD_DMA_Interface LCD_In(	.i_LCD_Control_CPU		(LCD_Control_CPU),
										.i_LCD_Data_CPU			(LCD_Data_CPU),
										.i_clk						(clk_100Mhz),
										.i_lcd_dma_chipselect	(lcd_dma_chipselect),
										.i_lcd_dma_write_n		(lcd_dma_write_n),
										.i_lcd_dma_writedata		(lcd_dma_writedata),
										.o_lcd_dma_waitrequest	(lcd_dma_waitrequest),
										.o_LCD_Control				(o_LCD_Control),
										.o_LCD_Data					(o_LCD_Data));
	
	//SPI DMA Interface
	wire [4:0]	spi_dma_address;
	wire       	spi_dma_chipselect;
	wire       	spi_dma_read_n;
	wire [7:0] 	spi_dma_readdata;
	wire        spi_dma_readdatavalid;
	wire        spi_dma_waitrequest;
	
	wire        spi_MISO_CPU;
	wire       	spi_MOSI_CPU;
	wire    		spi_SCLK_CPU;
	wire       	spi_SS_n_CPU;
	
	SPI_DMA_Interface SPI_In(	.i_clk						(clk_100Mhz),
										.i_rst						(cpuReset),
										.i_dma_in_address			(spi_dma_address),
										.i_dma_in_chipselect		(spi_dma_chipselect),
										.i_dma_in_read_n			(spi_dma_read_n),
										.o_dma_in_readdata		(spi_dma_readdata),
										.o_dma_in_readdatavalid	(spi_dma_readdatavalid),
										.o_dma_in_waitrequest	(spi_dma_waitrequest),
	
										.o_spi_MISO_CPU	(spi_MISO_CPU),
										.i_spi_MOSI_CPU	(spi_MOSI_CPU),
										.i_spi_SCLK_CPU	(spi_SCLK_CPU),
										.i_spi_SS_n_CPU	(spi_SS_n_CPU),
	
										.i_spi_MISO	(sd_spi_MISO),
										.o_spi_MOSI	(sd_spi_MOSI),
										.o_spi_SCLK	(sd_spi_SCLK),
										.o_spi_SS_n	(sd_spi_SS_n));
	
	
	//Keypad interface
	wire	[7:0]	keypad_KeyPressed;
	Keypad keypad(	.i_clk(clk_2Khz),
						.i_rows(i_KeypadRows),
						.o_columns(o_keypadColumns),
						.o_KeyPressed(keypad_KeyPressed)
						);
	
	//Channel enable buttons
	wire [1:0] btn_chONOFF;
	assign btn_chONOFF[0] = i_btn_CH0_ONOFF;
	assign btn_chONOFF[1] = i_btn_CH1_ONOFF;
	
	//DDS reset
	wire	[1:0] DDS_Reset;
	assign 	DDS0_Reset = DDS_Reset[0];
	assign 	DDS1_Reset = DDS_Reset[1];
	
	
	//NIOS II Procesor
	NiosII_Processor CPU(	.clk_clk				(clk_100Mhz),	//	CPU clock
									.cpu_reset			(cpuReset),		// CPU reset
									.led_debug_export	(o_LED_debug),	// Debug led on PCB
									.dds_reset_export	(DDS_Reset),	// DDS modules reset
									
									//DDS0 Controll signals
									.dds0_phasestep_export					(DDS0_DesiredPhaseStep),
									.dds0_phaseoffset_export				(DDS0_PM_PhaseOffset),
									.dds0_am_modphasestep_export			(DDS0_AM_ModulationPhaseStep),
									.dds0_am_modindex_export				(DDS0_AM_ModulationIndex),
									.dds0_fm_modphasestep_export			(DDS0_FM_ModulationPhaseStep),
									.dds0_fm_moddeviationphase_export	(DDS0_FM_ModulationDeviationPhase),
									.dds0_pm_modphasestep_export			(DDS0_PM_ModulationPhaseStep),
									.dds0_pm_modindex_export				(DDS0_PM_ModulationIndex),
									.dds0_pwm_amplitude_export				(PMW_DDS0_Amplitude_DutyCycle),
									.dds0_pwm_offset_export					(PMW_DDS0_Offset_DutyCycle),
									.dds0_outputrelay_export				(Relay_DDS0),
									
									//DDS1 Controll signals
									.dds1_phasestep_export					(DDS1_DesiredPhaseStep),
									.dds1_phaseoffset_export				(DDS1_PM_PhaseOffset),
									.dds1_am_modphasestep_export			(DDS1_AM_ModulationPhaseStep),
									.dds1_am_modindex_export				(DDS1_AM_ModulationIndex),
									.dds1_fm_modphasestep_export			(DDS1_FM_ModulationPhaseStep),
									.dds1_fm_moddeviationphase_export	(DDS1_FM_ModulationDeviationPhase),
									.dds1_pm_modphasestep_export			(DDS1_PM_ModulationPhaseStep),
									.dds1_pm_modindex_export				(DDS1_PM_ModulationIndex),
									.dds1_pwm_amplitude_export				(PMW_DDS1_Amplitude_DutyCycle),
									.dds1_pwm_offset_export					(PMW_DDS1_Offset_DutyCycle),
									.dds1_outputrelay_export				(Relay_DDS1),
									
									//LCD
									.lcd_data_export				(LCD_Data_CPU),
									.lcd_control_export			(LCD_Control_CPU), 
									.lcd_backlight_pwm_export	(PMW_LCD_BackLight_DutyCycle),
									.lcd_dma_address				(lcd_dma_address),
									.lcd_dma_chipselect			(lcd_dma_chipselect),
									.lcd_dma_waitrequest			(lcd_dma_waitrequest),
									.lcd_dma_write_n				(lcd_dma_write_n),
									.lcd_dma_writedata			(lcd_dma_writedata),
									
									//Keypad
									.keypad_input_export(keypad_KeyPressed),
									
									//Buttons and encoder
									.btn_display_export(i_btnDisplay),
									.btn_encoder_export(i_Encoder),
									.btn_ch_onoff_export(btn_chONOFF),
										
									//RAM used as lookup tables
									.ram_dds0_clk_clk			(clk_200Mhz),
									.ram_dds0_reset_reset_n	(1),
									.ram_dds0_address			(ram_dds0_address),
									.ram_dds0_chipselect		(1),
									.ram_dds0_clken			(1),
									.ram_dds0_write			(0),
									.ram_dds0_readdata		(ram_dds0_readdata),
									.ram_dds0_writedata		(0),
									.ram_dds0_byteenable		(2'h3),
									.ram_dds0_am_address		(ram_dds0_AM_address),
									.ram_dds0_am_chipselect	(1),
									.ram_dds0_am_clken		(1),
									.ram_dds0_am_write		(0),
									.ram_dds0_am_readdata	(ram_dds0_AM_readdata),
									.ram_dds0_am_writedata	(0),
									.ram_dds0_am_byteenable	(2'h3),
									.ram_dds0_fm_address		(ram_dds0_FM_address),
									.ram_dds0_fm_chipselect	(1),
									.ram_dds0_fm_clken		(1),
									.ram_dds0_fm_write		(0),
									.ram_dds0_fm_readdata	(ram_dds0_FM_readdata),
									.ram_dds0_fm_writedata	(0),
									.ram_dds0_fm_byteenable	(2'h3),

									.ram_dds1_address			(ram_dds1_address),
									.ram_dds1_chipselect		(1),
									.ram_dds1_clken			(1),
									.ram_dds1_write			(0),
									.ram_dds1_readdata		(ram_dds1_readdata),
									.ram_dds1_writedata		(0),
									.ram_dds1_byteenable		(2'h3),
									.ram_dds1_am_address		(ram_dds1_AM_address),
									.ram_dds1_am_chipselect	(1),
									.ram_dds1_am_clken		(1),
									.ram_dds1_am_write		(0),
									.ram_dds1_am_readdata	(ram_dds1_AM_readdata),
									.ram_dds1_am_writedata	(0),
									.ram_dds1_am_byteenable	(2'h3),
									.ram_dds1_fm_address		(ram_dds1_FM_address),
									.ram_dds1_fm_chipselect	(1),
									.ram_dds1_fm_clken		(1),
									.ram_dds1_fm_write		(0),
									.ram_dds1_fm_readdata	(ram_dds1_FM_readdata),
									.ram_dds1_fm_writedata	(0),
									.ram_dds1_fm_byteenable	(2'h3),
									.lookup_ram_isr_1_export(lookup_ram_isr),
									
									//SPI
									.spi_dma_address			(spi_dma_address),
									.spi_dma_chipselect		(spi_dma_chipselect),
									.spi_dma_read_n			(spi_dma_read_n),
									.spi_dma_readdata			(spi_dma_readdata),
									.spi_dma_readdatavalid	(spi_dma_readdatavalid),
									.spi_dma_waitrequest 	(spi_dma_waitrequest),
									.sd_spi_MISO(spi_MISO_CPU),
									.sd_spi_MOSI(spi_MOSI_CPU),
									.sd_spi_SCLK(spi_SCLK_CPU),
									.sd_spi_SS_n(spi_SS_n_CPU)
								);
							
								
endmodule