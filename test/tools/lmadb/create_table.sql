; RUN: lmadb -s %s

create table foo(
    id int8,
    a int8
);

; CHECK: foo
.tables

; CHECK: a int8
; CHECK-NEXT: id int8
.schema foo

; CHECK: Error: cannot insert table 'foo', already exists.
create table foo(id int8, a int8);

; CHECK: Error: cannot create table, column 'id' is not unique.
create table bar(id int8, id int8);
