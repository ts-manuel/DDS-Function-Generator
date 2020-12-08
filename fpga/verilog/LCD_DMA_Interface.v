/* 
	Project:	DDS Function Generator
	File:		LCD_DMA_Interface.v
	Date:		04/05/2018
	Autor:	Manuel
	
	Interface between LCD and DMA controller
*/

module LCD_DMA_Interface(
	input wire [4:0]	i_LCD_Control_CPU,		//Control signals from CPU
	input wire [7:0]	i_LCD_Data_CPU,			//Data signals from CPU
	input wire			i_clk,						//DMA clock
	input wire 			i_lcd_dma_chipselect,	//DMA Chip Select
	input wire 			i_lcd_dma_write_n,		//DMA Write request (active LOW)
	input wire [7:0]	i_lcd_dma_writedata,		//DMA Data
	output reg			o_lcd_dma_waitrequest,	//DMA waitreques
	output reg [4:0]	o_LCD_Control,				//LCD controll signals
	output reg [7:0]	o_LCD_Data					//LCD data signals
);

	reg 			transmitterBusy 	= 0;	//Flag set at the begining of the transmisssion
	reg 			transmissionDone 	= 0;	//Flag set at the end of the transmission
	reg			pageAddressSet		= 0;	//Flag set after sending the page address
	reg [7:0]	data;							//Data to be written to the LCD
	reg [3:0]	step 					= 0;
	integer 		counter 				= 0;
	integer		byteCounter			= 0;
	//reg [7:0]	pageAddress			= 8'hB0;
	
	always @(negedge i_clk) begin
	//Read data and set waitrequest
		if(!i_lcd_dma_write_n && !transmitterBusy && i_lcd_dma_chipselect && !transmissionDone) begin
			o_lcd_dma_waitrequest	<= 1;
			transmitterBusy 			<= 1;
			data 							<= i_lcd_dma_writedata;
		end
		
	//Start data transmission
		if(transmitterBusy) begin	//If the transmission is in progress
			if((counter % 32) == 0 && counter != 0) begin	//Slow the clock down by 20
				//Set page address
				if((byteCounter % 128) == 0 && !pageAddressSet) begin
					if(step == 0)begin
						o_LCD_Control 	<= 5'b10010;							//E = 1, RW = 0, A0 = 0, RST = 1, CS = 0
						o_LCD_Data		<= 8'hB0 + (byteCounter / 128);	//Set page address
						//o_LCD_Data 		<= pageAddress;
						//pageAddress		<= pageAddress	+ 1;
						step 				<= step 			+ 1;
					end
					else if(step == 2)begin
						o_LCD_Control 	<= 5'b10010;	//E = 1, RW = 0, A0 = 0, RST = 1, CS = 0
						o_LCD_Data		<= 8'h10;		//Set column address MSB
						step 				<= step + 1;
					end
					else if(step == 4)begin
						o_LCD_Control 	<= 5'b10010;	//E = 1, RW = 0, A0 = 0, RST = 1, CS = 0
						o_LCD_Data		<= 8'h04;		//Set column address LSB
						step 				<= step + 1;
					end
					else begin
						o_LCD_Control 	<= 5'b00011;	//E = 0, RW = 0, A0 = 0, RST = 1, CS = 1
						if(step >= 5) begin
							step 				<= 0;
							pageAddressSet	<= 1;
						end
						else
							step <= step + 1;
					end
				end
				//Send data
				else begin
					if(step == 0 || step == 2 || step == 4 || step == 6)begin
						o_LCD_Control 	<= 5'b10110;	//E = 1, RW = 0, A0 = 1, RST = 1, CS = 0
						o_LCD_Data		<= data;
					end
					else begin
						o_LCD_Control 	<= 5'b00111;	//E = 0, RW = 0, A0 = 1, RST = 1, CS = 1						
						if(step >= 7) begin
							transmitterBusy 	<= 0;
							transmissionDone	<= 1;
							pageAddressSet		<= 0;
							byteCounter			<= byteCounter + 1;
						end
					end
					step <= step + 1;
				end
			end
			counter <= counter + 1;
		end
		
	//End of transission
		if(transmissionDone) begin
			o_lcd_dma_waitrequest	<= 0;
			transmissionDone 			<= 0;
			counter 						<= 0;
			step							<= 0;
		end
		
	//CPU has controll over the buss
		if(!transmitterBusy && !i_lcd_dma_chipselect) begin	//If no transaction in progress and chipselect == 0
			o_LCD_Control	<= i_LCD_Control_CPU;					//The CPU has controll over the LCD buss
			o_LCD_Data 		<= i_LCD_Data_CPU;
			byteCounter		<= 0;
			//pageAddress		<= 8'hB0;
		end
	end


endmodule