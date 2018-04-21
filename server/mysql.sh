show databases;
use mysql;
create table user_info( 
	id INT NOT NULL AUTO_INCREMENT, 
	name VARCHAR(40) NOT NULL, 
	passwd VARCHAR(15) NOT NULL, 
	rtime VARCHAR(20) NOT NULL, 
	status VARCHAR(20) NOT NULL, 
	type VARCHAR(20) NOT NULL ,
	date DATE,
	PRIMARY KEY (id));
show tables;
describe table_name;
drop table table_name;
select * from table;
select count(*) from table;
select * from table;
select * from table where index = 1;
select * from table where index = 1 and name = "renxl";

