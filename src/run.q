init:`:./feedhandler 2:(`init;1)
halt:`:./feedhandler 2:(`halt;1)

trade:([]time:`timestamp$(); sym:`g#`symbol$(); exg:`symbol$(); size:(); volume:(); sequence:(); price:(); cond:());
quote:([]time:`timestamp$(); sym:`g#`symbol$(); exg:`symbol$(); asksize:(); bidsize:(); askprice:(); bidprice:(); sequence:(); cond:());

.u.upd:{[t;x] insert[t] each .z.p,'flip x; }