PROGRAM TEST1;
BEGIN
	CASE 1 * 2 / 3 MOD VAR OF
		10 MOD 4 : / LOOP ENDLOOP; \
		10 MOD 4 * 100 : /\
	ENDCASE;
	LOOP
	ENDLOOP;
END.