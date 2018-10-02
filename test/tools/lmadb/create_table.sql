-- RUN: lmadb -s %s

create table foo(
    id int8,
    a boolean
);

-- CHECK: foo
.tables;

-- TODO: reenable this test.
-- order should be the same as it was created.
-- xCHECK: id int8
-- xCHECK-NEXT: a boolean
-- .schema foo;

-- CHECK: Error: cannot insert table 'foo', already exists.
create table foo(id int8, a int8);

-- CHECK: Error: cannot create table, column 'id' is not unique.
create table bar(id int8, id int8);
