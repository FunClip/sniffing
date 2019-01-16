create table requests(
    id int not null comment '请求的id号',
    src_ip varchar(128) default '' not null comment '源ip地址',
    dest_ip varchar(128) default '' not null comment '目标ip地址',
    cap_time timestamp not null default current_timestamp on update current_timestamp comment '入库时间',
    primary key(id)
)CHARSET utf8;

create table request_headers(
    line_id int not null auto_increment comment '行id,不需要手动插入',
    req_id int not null comment '表requests中的请求id号',
    header_key varchar(256) default '' not null comment '首部行的键',
    header_value varchar(1024) default '' not null comment '首部行的值',
    primary key(line_id),
    foreign key(req_id) references requests(id)
)CHARSET utf8;