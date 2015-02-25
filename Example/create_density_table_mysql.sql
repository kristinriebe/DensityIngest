use TestDB;

create table Density (
    webId bigint not null,		
    ix int not null,
    iy int not null,
    iz int not null,
    phkey int null,
    dens float not null,
    snapnum int not null
)

