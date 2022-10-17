mtype = {down, up, left, right}
typedef struct {
	int a;
	bool b;
	mtype t;
}

active proctype test(){
	struct s;
	mtype t = right;
	t = up;
	s.t = t;
}
