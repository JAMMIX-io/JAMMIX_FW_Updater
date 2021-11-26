module wave_gen_sin(
	input clk,
	output reg [15:0] sine
);


reg [5:0] ramp;
always @(posedge clk) ramp <= ramp + 1;


always@(ramp[5:0])
begin
	case(ramp[5:0])
		0: sine <= 16'h0;
		1: sine <= 16'h322;
		2: sine <= 16'h63D;
		3: sine <= 16'h948;
		4: sine <= 16'hC3D;
		5: sine <= 16'hF13;
		6: sine <= 16'h11C5;
		7: sine <= 16'h144A;
		8: sine <= 16'h169E;
		9: sine <= 16'h18B9;
		10: sine <= 16'h1A98;
		11: sine <= 16'h1C36;
		12: sine <= 16'h1D8E;
		13: sine <= 16'h1E9D;
		14: sine <= 16'h1F61;
		15: sine <= 16'h1FD7;
		16: sine <= 16'h1FFF;
		17: sine <= 16'h1FD8;
		18: sine <= 16'h1F63;
		19: sine <= 16'h1EA1;
		20: sine <= 16'h1D93;
		21: sine <= 16'h1C3C;
		22: sine <= 16'h1AA0;
		23: sine <= 16'h18C2;
		24: sine <= 16'h16A7;
		25: sine <= 16'h1454;
		26: sine <= 16'h11CF;
		27: sine <= 16'hF1F;
		28: sine <= 16'hC49;
		29: sine <= 16'h955;
		30: sine <= 16'h64A;
		31: sine <= 16'h32F;
		32: sine <= 16'hD;
		33: sine <= 16'hFFFFFCEA;
		34: sine <= 16'hFFFFF9CF;
		35: sine <= 16'hFFFFF6C3;
		36: sine <= 16'hFFFFF3CE;
		37: sine <= 16'hFFFFF0F7;
		38: sine <= 16'hFFFFEE45;
		39: sine <= 16'hFFFFEBBF;
		40: sine <= 16'hFFFFE96B;
		41: sine <= 16'hFFFFE74E;
		42: sine <= 16'hFFFFE56E;
		43: sine <= 16'hFFFFE3CF;
		44: sine <= 16'hFFFFE276;
		45: sine <= 16'hFFFFE166;
		46: sine <= 16'hFFFFE0A1;
		47: sine <= 16'hFFFFE029;
		48: sine <= 16'hFFFFE000;
		49: sine <= 16'hFFFFE025;
		50: sine <= 16'hFFFFE099;
		51: sine <= 16'hFFFFE15A;
		52: sine <= 16'hFFFFE267;
		53: sine <= 16'hFFFFE3BD;
		54: sine <= 16'hFFFFE558;
		55: sine <= 16'hFFFFE735;
		56: sine <= 16'hFFFFE94F;
		57: sine <= 16'hFFFFEBA1;
		58: sine <= 16'hFFFFEE25;
		59: sine <= 16'hFFFFF0D5;
		60: sine <= 16'hFFFFF3AA;
		61: sine <= 16'hFFFFF69E;
		62: sine <= 16'hFFFFF9A9;
		63: sine <= 16'hFFFFFCC3;
		default: sine <= 0;
	endcase
end

endmodule
