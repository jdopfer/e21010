SIGNAL(ZERO_SUPPRESS: U1F_1);
SIGNAL(ZERO_SUPPRESS: U1B_1);
SIGNAL(ZERO_SUPPRESS: U2F_1);
SIGNAL(ZERO_SUPPRESS: U2B_1);
SIGNAL(ZERO_SUPPRESS: U3F_1);
SIGNAL(ZERO_SUPPRESS: U3B_1);
SIGNAL(ZERO_SUPPRESS: U4F_1);
SIGNAL(ZERO_SUPPRESS: U4B_1);
SIGNAL(ZERO_SUPPRESS: U5F_1);
SIGNAL(ZERO_SUPPRESS: U5B_1);
SIGNAL(ZERO_SUPPRESS: U6F_1);
SIGNAL(ZERO_SUPPRESS: U6B_1);


SIGNAL(U1F_1_E, ev.madc0[0].data[7],
       U1F_8_E, ev.madc0[0].data[0], (DATA14, float));
SIGNAL(U1F_9_E, ev.madc0[0].data[8],
       U1F_16_E, ev.madc0[0].data[15], (DATA14, float));

SIGNAL(U1B_1_E, ev.madc0[0].data[23],
       U1B_8_E, ev.madc0[0].data[16], (DATA14, float));
SIGNAL(U1B_9_E, ev.madc0[0].data[24],
       U1B_16_E, ev.madc0[0].data[31], (DATA14, float));


SIGNAL(U2F_1_E, ev.madc[0].data[7],
       U2F_8_E, ev.madc[0].data[0], (DATA14, float));
SIGNAL(U2F_9_E, ev.madc[0].data[8],
       U2F_16_E, ev.madc[0].data[15], (DATA14, float));

SIGNAL(U2B_1_E, ev.madc[0].data[23],
       U2B_8_E, ev.madc[0].data[16], (DATA14, float));
SIGNAL(U2B_9_E, ev.madc[0].data[24],
       U2B_16_E, ev.madc[0].data[31], (DATA14, float));


SIGNAL(U3F_1_E, ev.madc[1].data[7],
       U3F_8_E, ev.madc[1].data[0], (DATA14, float));
SIGNAL(U3F_9_E, ev.madc[1].data[8],
       U3F_16_E, ev.madc[1].data[15], (DATA14, float));

SIGNAL(U3B_1_E, ev.madc[1].data[23],
       U3B_8_E, ev.madc[1].data[16], (DATA14, float));
SIGNAL(U3B_9_E, ev.madc[1].data[24],
       U3B_16_E, ev.madc[1].data[31], (DATA14, float));


SIGNAL(U4F_1_E, ev.madc0[1].data[7],
       U4F_8_E, ev.madc0[1].data[0], (DATA14, float));
SIGNAL(U4F_9_E, ev.madc0[1].data[8],
       U4F_16_E, ev.madc0[1].data[15], (DATA14, float));

SIGNAL(U4B_1_E, ev.madc0[1].data[23],
       U4B_8_E, ev.madc0[1].data[16], (DATA14, float));
SIGNAL(U4B_9_E, ev.madc0[1].data[24],
       U4B_16_E, ev.madc0[1].data[31], (DATA14, float));


SIGNAL(U5F_1_E, ev.madc0[2].data[7],
       U5F_8_E, ev.madc0[2].data[0], (DATA14, float));
SIGNAL(U5F_9_E, ev.madc0[2].data[8],
       U5F_16_E, ev.madc0[2].data[15], (DATA14, float));

SIGNAL(U5B_1_E, ev.madc0[2].data[23],
       U5B_8_E, ev.madc0[2].data[16], (DATA14, float));
SIGNAL(U5B_9_E, ev.madc0[2].data[24],
       U5B_16_E, ev.madc0[2].data[31], (DATA14, float));


SIGNAL(U6F_1_E, ev.madc[2].data[7],
       U6F_8_E, ev.madc[2].data[0], (DATA14, float));
SIGNAL(U6F_9_E, ev.madc[2].data[8],
       U6F_16_E, ev.madc[2].data[15], (DATA14, float));

SIGNAL(U6B_1_E, ev.madc[2].data[23],
       U6B_8_E, ev.madc[2].data[16], (DATA14, float));
SIGNAL(U6B_9_E, ev.madc[2].data[24],
       U6B_16_E, ev.madc[2].data[31], (DATA14, float));


SIGNAL(P1_E, ev.madc0[3].data[0], (DATA14, float));
SIGNAL(P2_E, ev.madc0[3].data[2], (DATA14, float));
SIGNAL(P3_E, ev.madc0[3].data[4], (DATA14, float));
SIGNAL(P4_E, ev.madc0[3].data[6], (DATA14, float));
SIGNAL(P5_E, ev.madc0[3].data[8], (DATA14, float));
SIGNAL(P6_E, ev.madc0[3].data[10], (DATA14, float));


SIGNAL(G1_E, ev.madc0[3].data[24], (DATA14, float));
SIGNAL(G2_E, ev.madc0[3].data[28], (DATA14, float));


SIGNAL(U1B_1_T, ev.tdc[0].data[7],
       U1B_8_T, ev.tdc[0].data[0], (DATA24, float));
SIGNAL(U1B_9_T, ev.tdc[0].data[8],
       U1B_16_T, ev.tdc[0].data[15], (DATA24, float));


SIGNAL(U2B_1_T, ev.tdc[0].data[23],
       U2B_8_T, ev.tdc[0].data[16], (DATA24, float));
SIGNAL(U2B_9_T, ev.tdc[0].data[24],
       U2B_16_T, ev.tdc[0].data[31], (DATA24, float));


SIGNAL(U3B_1_T, ev.tdc[0].data[39],
       U3B_8_T, ev.tdc[0].data[32], (DATA24, float));
SIGNAL(U3B_9_T, ev.tdc[0].data[40],
       U3B_16_T, ev.tdc[0].data[47], (DATA24, float));


SIGNAL(U4B_1_T, ev.tdc[0].data[55],
       U4B_8_T, ev.tdc[0].data[48], (DATA24, float));
SIGNAL(U4B_9_T, ev.tdc[0].data[56],
       U4B_16_T, ev.tdc[0].data[63], (DATA24, float));


SIGNAL(U5B_1_T, ev.tdc[0].data[71],
       U5B_8_T, ev.tdc[0].data[64], (DATA24, float));
SIGNAL(U5B_9_T, ev.tdc[0].data[72],
       U5B_16_T, ev.tdc[0].data[79], (DATA24, float));


SIGNAL(U6B_1_T, ev.tdc[0].data[87],
       U6B_8_T, ev.tdc[0].data[80], (DATA24, float));
SIGNAL(U6B_9_T, ev.tdc[0].data[88],
       U6B_16_T, ev.tdc[0].data[95], (DATA24, float));


SIGNAL(P1_T, ev.tdc[0].data[96], (DATA24, float));
SIGNAL(P2_T, ev.tdc[0].data[98], (DATA24, float));
SIGNAL(P3_T, ev.tdc[0].data[100], (DATA24, float));
SIGNAL(P4_T, ev.tdc[0].data[102], (DATA24, float));
SIGNAL(P5_T, ev.tdc[0].data[104], (DATA24, float));
SIGNAL(P6_T, ev.tdc[0].data[106], (DATA24, float));


SIGNAL(G1_T, ev.tdc[0].data[120], (DATA24, float));
SIGNAL(G2_T, ev.tdc[0].data[124], (DATA24, float));
