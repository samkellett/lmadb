; RUN: lmadb -s %s

create table foo(
    id int8,
    a int8
);

; CHECK: Error: unimplemented.
insert into foo values(1, 2);
