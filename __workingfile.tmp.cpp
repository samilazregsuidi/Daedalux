# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
typedef vector {
 int x;
 int y;
 int z
}

typedef quaternion {
 int a;
 int i;
 int j;
 int w
}

typedef matrix {
 vector position;
 vector scale;
 quaternion orientation;
 byte id
}

active proctype test (){

 matrix obj;

 obj.id = 1;

 obj.position.x = -1;
 obj.position.y = 0;
 obj.position.z = 1;

 obj.orientation.i = obj.position.z;
 obj.orientation.w = -obj.orientation.i;

 obj.scale.y = obj.position.x * -obj.orientation.i;

 assert(obj.position.x == -1 && obj.position.y == 0 && obj.position.z == 1);
 assert(obj.orientation.i == 1 && obj.orientation.w == -1);
 assert(obj.scale.y == 1);
 assert((obj.position.x * obj.position.y * obj.position.z * obj.orientation.i * obj.orientation.w * obj.orientation.a * obj.orientation.j * obj.scale.z) == 0);
}
