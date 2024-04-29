DROP SCHEMA IF EXISTS hello_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS hello_schema;

CREATE TABLE IF NOT EXISTS hello_schema.users (
    name TEXT PRIMARY KEY,
    count INTEGER DEFAULT(1)
);




			    CREATE TABLE IF NOT EXISTS CouriersData 
			    (
				id SERIAL PRIMARY KEY, 
				courier_json VARCHAR,
				json_hash CHAR(64) UNIQUE

			    );
			    
			    			    CREATE TABLE IF NOT EXISTS OrdersData (

						    id SERIAL PRIMARY KEY,    
						    order_json JSON,
						    json_hash CHAR(64) UNIQUE,
						    completed_time TEXT 
						                   );


								create table if not exists complete_info (
								    courier_id int,
								    order_id int,
								    complete_time text,

								    foreign key (order_id) references OrdersData(id),
								    foreign key (courier_id) references CouriersData(id),
								    primary key (order_id)
								);
