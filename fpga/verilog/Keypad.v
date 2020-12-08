/* 
	Project:	DDS Function Generator
	File:		Keypad.v
	Date:		31/03/2018
	Autor:	Manuel
	
	keypad scanning module
*/

module Keypad(
	input				i_clk,				//Clock
	input		[3:0]	i_rows,				//Keypad rows input
	output	[3:0]	o_columns,			//keypad columnd output
	output	[7:0]	o_KeyPressed		//Last pressed key code
);
	
	localparam _KEY_PRESS_DALAY = 8'd10, _KEY_RELEASE_DALAY = 8'd20;
	localparam _STATE_IDLE = 4'b0001, _STATE_KEY_PRESSED = 4'b0010,
				  _STATE_WAIT_RELEASE = 4'b0100, _STATE_KEY_RELEASED = 4'b1000;
	
	reg [7:0] 	KeyPressed		= 8'hFF;			//Key code detected as a valid press
	reg [7:0] 	KeyCode 			= 8'h00;			//Key code read during matrix scan
	reg [7:0] 	DelayCounter	= 8'h00;			//Counter used to add debounce delay
	reg [2:0] 	ColumnsCounter = 3'h00;			//Conter used to scan the columns
	reg [2:0]	NumberInactiveRows = 3'h00;	//Number of inactive columns found during the keypad scan
	reg [3:0] 	CurrentState	= _STATE_IDLE;	//Current state machine state
	reg [3:0]	NextState;							//Next state calculated by the comb logic
	
	assign o_KeyPressed = KeyPressed;
	assign o_columns = ~(4'b0001 << ColumnsCounter);	//Columns active low
	
	
	//Combinatory logic that calculates the next state
	always @(CurrentState or KeyCode or DelayCounter) begin
		case(CurrentState)
			_STATE_IDLE:
				if(KeyCode != 8'hFF)						//If a key is been pressed
					NextState = _STATE_KEY_PRESSED;
				else
					NextState = _STATE_IDLE;
			_STATE_KEY_PRESSED:
				if(DelayCounter > _KEY_PRESS_DALAY)
					NextState = _STATE_WAIT_RELEASE;
				else
					NextState = _STATE_KEY_PRESSED;
			_STATE_WAIT_RELEASE:
				if(KeyCode == 8'hFF)
					NextState = _STATE_KEY_RELEASED;
				else
					NextState = _STATE_WAIT_RELEASE;
			_STATE_KEY_RELEASED:
				if(DelayCounter > _KEY_RELEASE_DALAY)
					NextState = _STATE_IDLE;
				else
					NextState = _STATE_KEY_RELEASED;
		endcase
	end
	
	//Scan matrix
	always @(posedge i_clk) begin
		case(ColumnsCounter)
			3'd0: begin
				case(i_rows)
					4'b1110:	KeyCode <= 8'h0D;	//D
					4'b1101: KeyCode <= 8'h0C;	//C
					4'b1011: KeyCode <= 8'h0B;	//B
					4'b0111: KeyCode <= 8'h0A;	//A
					default:	NumberInactiveRows <= NumberInactiveRows + 3'h01;
				endcase
			end
			3'd1: begin
				case(i_rows)
					4'b1110:	KeyCode <= 8'h10;	//#
					4'b1101: KeyCode <= 8'h09;	//9
					4'b1011: KeyCode <= 8'h06;	//6
					4'b0111: KeyCode <= 8'h03;	//3
					default:	NumberInactiveRows <= NumberInactiveRows + 3'h01;
				endcase
			end
			3'd2: begin
				case(i_rows)
					4'b1110:	KeyCode <= 8'h00;	//0
					4'b1101: KeyCode <= 8'h08;	//8
					4'b1011: KeyCode <= 8'h05;	//5
					4'b0111: KeyCode <= 8'h02;	//2
					default:	NumberInactiveRows <= NumberInactiveRows + 3'h01;
				endcase
			end
			3'd3: begin
				case(i_rows)
					4'b1110:	KeyCode <= 8'h11;	//*
					4'b1101: KeyCode <= 8'h07;	//7
					4'b1011: KeyCode <= 8'h04;	//4
					4'b0111: KeyCode <= 8'h01;	//1
					default:	NumberInactiveRows <= NumberInactiveRows + 3'h01;
				endcase
			end
			3'd4: begin
				if(NumberInactiveRows == 4)
					KeyCode <= 8'hFF;
				NumberInactiveRows <= 3'h00;
			end
		endcase
		
		ColumnsCounter <= ColumnsCounter + 3'd1;
	end
	
	//Process state machine
	always @(posedge i_clk) begin
		//Increment counter
		if(CurrentState == _STATE_KEY_PRESSED || CurrentState == _STATE_KEY_RELEASED)
			DelayCounter <= DelayCounter + 8'h01;
		else
			DelayCounter <= 8'h00;
		
		//Assign Key code to uotput
		if(CurrentState == _STATE_WAIT_RELEASE && KeyPressed == 8'hFF)
			KeyPressed <= KeyCode;
		else if(CurrentState == _STATE_IDLE)
			KeyPressed <= 8'hFF;
		
		CurrentState <= NextState;
	end
	
endmodule