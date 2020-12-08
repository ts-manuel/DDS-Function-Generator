/* 
	Project:	DDS Function Generator
	File:		PWM.v
	Date:		26/01/2018
	Autor:	Manuel
	
	PWM signal with frequency i_clk / 2^_NUM_BIT
	and with _NUM_BIT of resolution
*/

module PWM #(
	parameter	_NUM_BIT = 12
	)(
	input 						i_clk,
	input		[_NUM_BIT-1:0]	i_dutyCycle,
	output						o_PWM
	);
	
	reg [_NUM_BIT-1:0]	PWMCounter = 0;
	reg PWM = 0;
	assign o_PWM = PWM;
	
	always @(posedge i_clk) begin
		if(PWMCounter < i_dutyCycle)	PWM <= 1;
		else									PWM <= 0;
		
		PWMCounter <= PWMCounter + 1;
	end
	
endmodule