/* 
	Project:	DDS Function Generator
	File:		SPI_DMA_Interface.v
	Date:		26/01/2018
	Autor:	Manuel
	
	Interface between SPI bus and DMA controller
*/

module SPI_DMA_Interface(
	input wire 			i_clk,
	input wire			i_rst,
	input wire [4:0]	i_dma_in_address,
	input wire       	i_dma_in_chipselect,
	input wire       	i_dma_in_read_n,
	output reg [7:0] 	o_dma_in_readdata,
	output reg        o_dma_in_readdatavalid,
	output reg        o_dma_in_waitrequest,
	
	output reg        o_spi_MISO_CPU,
	input wire       	i_spi_MOSI_CPU,
	input wire       	i_spi_SCLK_CPU,
	input wire       	i_spi_SS_n_CPU,
	
	input wire        i_spi_MISO,
	output reg       	o_spi_MOSI,
	output reg       	o_spi_SCLK,
	output reg       	o_spi_SS_n
	);

	
	reg [7:0]	data 			= 0;
	reg 			dataReady 	= 0;
	reg [31:0] 	counter 		= 0;
	reg [3:0]	bitIndex 	= 0;
	
	
	always @(posedge i_clk) begin
		if(i_rst) begin
			data							<= 0;
			o_dma_in_readdatavalid 	<= 0;
			o_dma_in_waitrequest		<= 0;
			dataReady					<= 0;
			counter						<= 0;
			bitIndex						<= 0;
		end
		else begin
			if(!i_dma_in_read_n && i_dma_in_chipselect && !o_dma_in_waitrequest) begin //DMA selected
				o_dma_in_waitrequest	<= 1;
				dataReady 				<= 0;
				o_spi_SCLK				<= 0;
				o_spi_MOSI				<= 1;
			end
				
			if(o_dma_in_waitrequest && !o_dma_in_readdatavalid && dataReady) begin		//read data
				o_dma_in_readdata			<= data;
				o_dma_in_waitrequest		<= 0;
				o_dma_in_readdatavalid	<= 1;
			end
			else if(!o_dma_in_waitrequest && o_dma_in_readdatavalid) begin
				o_dma_in_readdatavalid	<= 0;
			end
				
			if(i_dma_in_read_n && !i_dma_in_chipselect && !o_dma_in_waitrequest && !o_dma_in_readdatavalid) begin	//Dma not selected
				data							<= 0;
				o_dma_in_readdatavalid 	<= 0;
				o_dma_in_waitrequest		<= 0;
				dataReady					<= 0;
				counter						<= 0;
				bitIndex						<= 0;
				
				o_spi_MISO_CPU <= i_spi_MISO;
				o_spi_MOSI		<= i_spi_MOSI_CPU;
				o_spi_SCLK		<= i_spi_SCLK_CPU;
				o_spi_SS_n		<= i_spi_SS_n_CPU;
			end
			
			if(o_dma_in_waitrequest && !dataReady) begin		//Read data from SPI
				if(counter % 2 == 0 && counter != 0) begin	//Devide clock rate by 16
					if(bitIndex < 8) begin
						if(!o_spi_SCLK) begin
							data[7 - bitIndex]	<= i_spi_MISO;
							bitIndex					<= bitIndex + 1;
						end
						o_spi_SCLK	<= !o_spi_SCLK;
					end
					else begin
						dataReady	<= 1;
						bitIndex 	<= 0;
					end
				end
					
				counter <= counter + 1;
			end
		end
	end
	
endmodule