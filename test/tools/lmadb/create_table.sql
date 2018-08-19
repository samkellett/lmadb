; RUN: lmadb -s %s

create table foo(id int8, a int8);

; CHECK: foo
.tables

; CHECK: a int8
; CHECK-NEXT: id int8
.schema foo

; CHECK: Error internal error
create table foo(id int8, a int8);

; CHECK: Error internal error
create table bar(id int8, id int8);
