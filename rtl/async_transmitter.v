// RS-232 TX module
// (c) fpga4fun.com KNJN LLC - 2003, 2004, 2005, 2006

//`define DEBUG   // in DEBUG mode, we output one bit per clock cycle (useful for faster simulations)

module async_transmitter(clk, TxD_start, TxD_data, TxD, TxD_busy);
input clk, TxD_start;
input [7:0] TxD_data;
output TxD, TxD_busy;


parameter ClkFrequency = 24000000;
parameter Baud = 57600;
parameter RegisterInputData = 1;	// in RegisterInputData mode, the input doesn't have to stay valid while the character is been transmitted

// Baud generator
parameter BaudGeneratorAccWidth = 16;
reg [BaudGeneratorAccWidth:0] BaudGeneratorAcc;
`ifdef DEBUG
wire [BaudGeneratorAccWidth:0] BaudGeneratorInc = 17'h10000;
`else
wire [BaudGeneratorAccWidth:0] BaudGeneratorInc = ((Baud<<(BaudGeneratorAccWidth-4))+(ClkFrequency>>5))/(ClkFrequency>>4);
`endif

(*keep*)wire BaudTick = BaudGeneratorAcc[BaudGeneratorAccWidth];
wire TxD_busy;
always @(posedge clk) if(TxD_busy) BaudGeneratorAcc <= BaudGeneratorAcc[BaudGeneratorAccWidth-1:0] + BaudGeneratorInc;

// Transmitter state machine
reg [4:0] state;
wire TxD_ready = (state==0);
assign TxD_busy = ~TxD_ready;

reg [7:0] TxD_dataReg;
always @(posedge clk) if(TxD_ready & TxD_start) TxD_dataReg <= TxD_data;
wire [7:0] TxD_dataD = RegisterInputData ? TxD_dataReg : TxD_data;

always @(posedge clk)
case(state)
	5'b00000: if(TxD_start) state <= 5'b00001;
	5'b00001: if(BaudTick)  state <= 5'b00100;
	5'b00100: if(BaudTick)  state <= 5'b10000;  // start
	
	5'b10000: if(BaudTick)  state <= 5'b10001;  // bit 0
	5'b10001: if(BaudTick)  state <= 5'b10010;  // bit 1
	5'b10010: if(BaudTick)  state <= 5'b10011;  // bit 2
	5'b10011: if(BaudTick)  state <= 5'b10100;  // bit 3
	5'b10100: if(BaudTick)  state <= 5'b10101;  // bit 4
	5'b10101: if(BaudTick)  state <= 5'b10110;  // bit 5
	5'b10110: if(BaudTick)  state <= 5'b10111;  // bit 6
	5'b10111: if(BaudTick)  state <= 5'b11000;  // bit 7
	5'b11000: if(BaudTick)  state <= 5'b00011;  // parity
	
	5'b00011: if(BaudTick)  state <= 5'b00000;  // stop
	
	 default: if(BaudTick)  state <= 5'b00000;
endcase

wire parity_bit = TxD_dataD[7] ^ TxD_dataD[6] ^ TxD_dataD[5] ^ TxD_dataD[4] ^ TxD_dataD[3] ^ TxD_dataD[2] ^ TxD_dataD[1] ^ TxD_dataD[0];

// Output mux
reg muxbit;
always @( * )
case(state[3:0])
	4'd0: muxbit <= TxD_dataD[0];
	4'd1: muxbit <= TxD_dataD[1];
	4'd2: muxbit <= TxD_dataD[2];
	4'd3: muxbit <= TxD_dataD[3];
	4'd4: muxbit <= TxD_dataD[4];
	4'd5: muxbit <= TxD_dataD[5];
	4'd6: muxbit <= TxD_dataD[6];
	4'd7: muxbit <= TxD_dataD[7];
	4'd8: muxbit <= parity_bit;
endcase


// Put together the start, data and stop bits
reg TxD;
always @(posedge clk) TxD <= (state<4) | (state[4] & muxbit);  // register the output to make it glitch free

endmodule