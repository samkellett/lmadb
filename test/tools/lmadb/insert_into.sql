; RUN: lmadb -s %s

create table foo(id int8, a boolean);

; CHECK: Error: unexpected type at index 0, expected int8.
insert into foo values(true, false);

; CHECK: Error: unexpected type at index 1, expected boolean.
insert into foo values(1, 2);

; CHECK: Error: unimplemented.
insert into foo values(1, true);
