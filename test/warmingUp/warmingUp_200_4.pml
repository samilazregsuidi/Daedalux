typedef features {
	bool B1;
	bool B2;
	bool B3;
	bool B4;
	bool B5;
	bool B6;
	bool B7;
	bool B8;
	bool B9;
	bool B10;
	bool B11;
	bool B12;
	bool B13;
	bool B14;
	bool B15;
	bool B16;
	bool B17;
	bool B18;
	bool B19;
	bool B20;
	bool B21;
	bool B22;
	bool B23;
	bool B24;
	bool B25;
	bool B26;
	bool B27;
	bool B28;
	bool B29;
	bool B30;
	bool B31;
	bool B32;
	bool B33;
	bool B34;
	bool B35;
	bool B36;
	bool B37;
	bool B38;
	bool B39;
	bool B40;
	bool B41;
	bool B42;
	bool B43;
	bool B44;
	bool B45;
	bool B46;
	bool B47;
	bool B48;
	bool B49;
	bool B50;
	bool B51;
	bool B52;
	bool B53;
	bool B54;
	bool B55;
	bool B56;
	bool B57;
	bool B58;
	bool B59;
	bool B60;
	bool B61;
	bool B62;
	bool B63;
	bool B64;
	bool B65;
	bool B66;
	bool B67;
	bool B68;
	bool B69;
	bool B70;
	bool B71;
	bool B72;
	bool B73;
	bool B74;
	bool B75;
	bool B76;
	bool B77;
	bool B78;
	bool B79;
	bool B80;
	bool B81;
	bool B82;
	bool B83;
	bool B84;
	bool B85;
	bool B86;
	bool B87;
	bool B88;
	bool B89;
	bool B90;
	bool B91;
	bool B92;
	bool B93;
	bool B94;
	bool B95;
	bool B96;
	bool B97;
	bool B98;
	bool B99;
	bool B100;
	bool B101;
	bool B102;
	bool B103;
	bool B104;
	bool B105;
	bool B106;
	bool B107;
	bool B108;
	bool B109;
	bool B110;
	bool B111;
	bool B112;
	bool B113;
	bool B114;
	bool B115;
	bool B116;
	bool B117;
	bool B118;
	bool B119;
	bool B120;
	bool B121;
	bool B122;
	bool B123;
	bool B124;
	bool B125;
	bool B126;
	bool B127;
	bool B128;
	bool B129;
	bool B130;
	bool B131;
	bool B132;
	bool B133;
	bool B134;
	bool B135;
	bool B136;
	bool B137;
	bool B138;
	bool B139;
	bool B140;
	bool B141;
	bool B142;
	bool B143;
	bool B144;
	bool B145;
	bool B146;
	bool B147;
	bool B148;
	bool B149;
	bool B150;
	bool B151;
	bool B152;
	bool B153;
	bool B154;
	bool B155;
	bool B156;
	bool B157;
	bool B158;
	bool B159;
	bool B160;
	bool B161;
	bool B162;
	bool B163;
	bool B164;
	bool B165;
	bool B166;
	bool B167;
	bool B168;
	bool B169;
	bool B170;
	bool B171;
	bool B172;
	bool B173;
	bool B174;
	bool B175;
	bool B176;
	bool B177;
	bool B178;
	bool B179;
	bool B180;
	bool B181;
	bool B182;
	bool B183;
	bool B184;
	bool B185;
	bool B186;
	bool B187;
	bool B188;
	bool B189;
	bool B190;
	bool B191;
	bool B192;
	bool B193;
	bool B194;
	bool B195;
	bool B196;
	bool B197;
	bool B198;
	bool B199;
	bool B200
}
features f;

system p1 = f.B1;
system p2 = f.B2;
system p3 = f.B3;
system p4 = f.B4;

byte n;
short i;

active proctype foo(){
	do
	::break;
	::n++;
	od;

Start: i = n;
	if::f.B1->i=i+200::else;fi;
	if::f.B2->i=i+199::else;fi;
	if::f.B3->i=i+198::else;fi;
	if::f.B4->i=i+197::else;fi;
	if::f.B5->i=i+196::else;fi;
	if::f.B6->i=i+195::else;fi;
	if::f.B7->i=i+194::else;fi;
	if::f.B8->i=i+193::else;fi;
	if::f.B9->i=i+192::else;fi;
	if::f.B10->i=i+191::else;fi;
	if::f.B11->i=i+190::else;fi;
	if::f.B12->i=i+189::else;fi;
	if::f.B13->i=i+188::else;fi;
	if::f.B14->i=i+187::else;fi;
	if::f.B15->i=i+186::else;fi;
	if::f.B16->i=i+185::else;fi;
	if::f.B17->i=i+184::else;fi;
	if::f.B18->i=i+183::else;fi;
	if::f.B19->i=i+182::else;fi;
	if::f.B20->i=i+181::else;fi;
	if::f.B21->i=i+180::else;fi;
	if::f.B22->i=i+179::else;fi;
	if::f.B23->i=i+178::else;fi;
	if::f.B24->i=i+177::else;fi;
	if::f.B25->i=i+176::else;fi;
	if::f.B26->i=i+175::else;fi;
	if::f.B27->i=i+174::else;fi;
	if::f.B28->i=i+173::else;fi;
	if::f.B29->i=i+172::else;fi;
	if::f.B30->i=i+171::else;fi;
	if::f.B31->i=i+170::else;fi;
	if::f.B32->i=i+169::else;fi;
	if::f.B33->i=i+168::else;fi;
	if::f.B34->i=i+167::else;fi;
	if::f.B35->i=i+166::else;fi;
	if::f.B36->i=i+165::else;fi;
	if::f.B37->i=i+164::else;fi;
	if::f.B38->i=i+163::else;fi;
	if::f.B39->i=i+162::else;fi;
	if::f.B40->i=i+161::else;fi;
	if::f.B41->i=i+160::else;fi;
	if::f.B42->i=i+159::else;fi;
	if::f.B43->i=i+158::else;fi;
	if::f.B44->i=i+157::else;fi;
	if::f.B45->i=i+156::else;fi;
	if::f.B46->i=i+155::else;fi;
	if::f.B47->i=i+154::else;fi;
	if::f.B48->i=i+153::else;fi;
	if::f.B49->i=i+152::else;fi;
	if::f.B50->i=i+151::else;fi;
	if::f.B51->i=i+150::else;fi;
	if::f.B52->i=i+149::else;fi;
	if::f.B53->i=i+148::else;fi;
	if::f.B54->i=i+147::else;fi;
	if::f.B55->i=i+146::else;fi;
	if::f.B56->i=i+145::else;fi;
	if::f.B57->i=i+144::else;fi;
	if::f.B58->i=i+143::else;fi;
	if::f.B59->i=i+142::else;fi;
	if::f.B60->i=i+141::else;fi;
	if::f.B61->i=i+140::else;fi;
	if::f.B62->i=i+139::else;fi;
	if::f.B63->i=i+138::else;fi;
	if::f.B64->i=i+137::else;fi;
	if::f.B65->i=i+136::else;fi;
	if::f.B66->i=i+135::else;fi;
	if::f.B67->i=i+134::else;fi;
	if::f.B68->i=i+133::else;fi;
	if::f.B69->i=i+132::else;fi;
	if::f.B70->i=i+131::else;fi;
	if::f.B71->i=i+130::else;fi;
	if::f.B72->i=i+129::else;fi;
	if::f.B73->i=i+128::else;fi;
	if::f.B74->i=i+127::else;fi;
	if::f.B75->i=i+126::else;fi;
	if::f.B76->i=i+125::else;fi;
	if::f.B77->i=i+124::else;fi;
	if::f.B78->i=i+123::else;fi;
	if::f.B79->i=i+122::else;fi;
	if::f.B80->i=i+121::else;fi;
	if::f.B81->i=i+120::else;fi;
	if::f.B82->i=i+119::else;fi;
	if::f.B83->i=i+118::else;fi;
	if::f.B84->i=i+117::else;fi;
	if::f.B85->i=i+116::else;fi;
	if::f.B86->i=i+115::else;fi;
	if::f.B87->i=i+114::else;fi;
	if::f.B88->i=i+113::else;fi;
	if::f.B89->i=i+112::else;fi;
	if::f.B90->i=i+111::else;fi;
	if::f.B91->i=i+110::else;fi;
	if::f.B92->i=i+109::else;fi;
	if::f.B93->i=i+108::else;fi;
	if::f.B94->i=i+107::else;fi;
	if::f.B95->i=i+106::else;fi;
	if::f.B96->i=i+105::else;fi;
	if::f.B97->i=i+104::else;fi;
	if::f.B98->i=i+103::else;fi;
	if::f.B99->i=i+102::else;fi;
	if::f.B100->i=i+101::else;fi;
	if::f.B101->i=i+100::else;fi;
	if::f.B102->i=i+99::else;fi;
	if::f.B103->i=i+98::else;fi;
	if::f.B104->i=i+97::else;fi;
	if::f.B105->i=i+96::else;fi;
	if::f.B106->i=i+95::else;fi;
	if::f.B107->i=i+94::else;fi;
	if::f.B108->i=i+93::else;fi;
	if::f.B109->i=i+92::else;fi;
	if::f.B110->i=i+91::else;fi;
	if::f.B111->i=i+90::else;fi;
	if::f.B112->i=i+89::else;fi;
	if::f.B113->i=i+88::else;fi;
	if::f.B114->i=i+87::else;fi;
	if::f.B115->i=i+86::else;fi;
	if::f.B116->i=i+85::else;fi;
	if::f.B117->i=i+84::else;fi;
	if::f.B118->i=i+83::else;fi;
	if::f.B119->i=i+82::else;fi;
	if::f.B120->i=i+81::else;fi;
	if::f.B121->i=i+80::else;fi;
	if::f.B122->i=i+79::else;fi;
	if::f.B123->i=i+78::else;fi;
	if::f.B124->i=i+77::else;fi;
	if::f.B125->i=i+76::else;fi;
	if::f.B126->i=i+75::else;fi;
	if::f.B127->i=i+74::else;fi;
	if::f.B128->i=i+73::else;fi;
	if::f.B129->i=i+72::else;fi;
	if::f.B130->i=i+71::else;fi;
	if::f.B131->i=i+70::else;fi;
	if::f.B132->i=i+69::else;fi;
	if::f.B133->i=i+68::else;fi;
	if::f.B134->i=i+67::else;fi;
	if::f.B135->i=i+66::else;fi;
	if::f.B136->i=i+65::else;fi;
	if::f.B137->i=i+64::else;fi;
	if::f.B138->i=i+63::else;fi;
	if::f.B139->i=i+62::else;fi;
	if::f.B140->i=i+61::else;fi;
	if::f.B141->i=i+60::else;fi;
	if::f.B142->i=i+59::else;fi;
	if::f.B143->i=i+58::else;fi;
	if::f.B144->i=i+57::else;fi;
	if::f.B145->i=i+56::else;fi;
	if::f.B146->i=i+55::else;fi;
	if::f.B147->i=i+54::else;fi;
	if::f.B148->i=i+53::else;fi;
	if::f.B149->i=i+52::else;fi;
	if::f.B150->i=i+51::else;fi;
	if::f.B151->i=i+50::else;fi;
	if::f.B152->i=i+49::else;fi;
	if::f.B153->i=i+48::else;fi;
	if::f.B154->i=i+47::else;fi;
	if::f.B155->i=i+46::else;fi;
	if::f.B156->i=i+45::else;fi;
	if::f.B157->i=i+44::else;fi;
	if::f.B158->i=i+43::else;fi;
	if::f.B159->i=i+42::else;fi;
	if::f.B160->i=i+41::else;fi;
	if::f.B161->i=i+40::else;fi;
	if::f.B162->i=i+39::else;fi;
	if::f.B163->i=i+38::else;fi;
	if::f.B164->i=i+37::else;fi;
	if::f.B165->i=i+36::else;fi;
	if::f.B166->i=i+35::else;fi;
	if::f.B167->i=i+34::else;fi;
	if::f.B168->i=i+33::else;fi;
	if::f.B169->i=i+32::else;fi;
	if::f.B170->i=i+31::else;fi;
	if::f.B171->i=i+30::else;fi;
	if::f.B172->i=i+29::else;fi;
	if::f.B173->i=i+28::else;fi;
	if::f.B174->i=i+27::else;fi;
	if::f.B175->i=i+26::else;fi;
	if::f.B176->i=i+25::else;fi;
	if::f.B177->i=i+24::else;fi;
	if::f.B178->i=i+23::else;fi;
	if::f.B179->i=i+22::else;fi;
	if::f.B180->i=i+21::else;fi;
	if::f.B181->i=i+20::else;fi;
	if::f.B182->i=i+19::else;fi;
	if::f.B183->i=i+18::else;fi;
	if::f.B184->i=i+17::else;fi;
	if::f.B185->i=i+16::else;fi;
	if::f.B186->i=i+15::else;fi;
	if::f.B187->i=i+14::else;fi;
	if::f.B188->i=i+13::else;fi;
	if::f.B189->i=i+12::else;fi;
	if::f.B190->i=i+11::else;fi;
	if::f.B191->i=i+10::else;fi;
	if::f.B192->i=i+9::else;fi;
	if::f.B193->i=i+8::else;fi;
	if::f.B194->i=i+7::else;fi;
	if::f.B195->i=i+6::else;fi;
	if::f.B196->i=i+5::else;fi;
	if::f.B197->i=i+4::else;fi;
	if::f.B198->i=i+3::else;fi;
	if::f.B199->i=i+2::else;fi;
	if::f.B200->i=i+1::else;fi;

Final: skip;
	do
	::true;
	od;
}
never{
T0_init :
	if
	::((!p1.foo@Final)||(!p2.foo@Final)||(!p3.foo@Final)||(!p4.foo@Final)||(!(p1.i>=p2.i))||(!(p2.i>=p3.i))||(!(p3.i>=p4.i)))&&((p1.foo@Start)&&(p2.foo@Start)&&(p3.foo@Start)&&(p4.foo@Start)&&(p1.n==p2.n)&&(p2.n==p3.n)&&(p3.n==p4.n))->goto accept_S2;
	::(1)->goto T0_init;
	fi;
accept_S2:
	if
	::(!p1.foo@Final)||(!p2.foo@Final)||(!p3.foo@Final)||(!p4.foo@Final)||(!(p1.i>=p2.i))||(!(p2.i>=p3.i))||(!(p3.i>=p4.i))->goto accept_S2;
	fi;
}