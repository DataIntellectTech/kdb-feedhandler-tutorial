init:`:./feedhandler 2:(`init;1)
halt:`:./feedhandler 2:(`halt;1)

trade:([]time:`timestamp$(); sym:`g#`symbol$(); exg:`symbol$(); size:(); volume:(); sequence:(); price:(); cond:());
quote:([]time:`timestamp$(); sym:`g#`symbol$(); exg:`symbol$(); asksize:(); bidsize:(); askprice:(); bidprice:(); sequence:(); cond:());

\p 7010
.u.count:0;
.u.upd:{[t;x] t insert raze .z.p,x; .u.count::.u.count+1; };

.z.ts:{ -1 "recieved ", (string .u.count), " updates."; .u.count:0; }

\t 1000
