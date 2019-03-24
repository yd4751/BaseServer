
use bighat;

DROP TABLE IF EXISTS UserInfo;
DROP TABLE IF EXISTS UserAuthInfo;
DROP TABLE IF EXISTS AuthType;

#用户信息表
CREATE TABLE UserInfo
(
    user_id INT PRIMARY KEY AUTO_INCREMENT, 
    nick_name NVARCHAR(128) DEFAULT "",
    avatar NVARCHAR(256) DEFAULT "",
    update_time DATETIME NOT NULL,
    money  BIGINT DEFAULT 0					 	#携带金币
)AUTO_INCREMENT=10000000;

#用户授权表,同一类型可绑定多个，但不允许重复绑定
CREATE TABLE UserAuthInfo
(
	user_id INT PRIMARY KEY NOT NULL,
    identity_id INT NOT NULL,		#第三方登录类型
    identifier NVARCHAR(256) NOT NULL UNIQUE,	#标识
    credential  NVARCHAR(256) NOT NULL,			#密码凭证
    verfiy	INT DEFAULT 0						#已验证标识 0-未验证 1-已验证
);

#授权类型
CREATE TABLE AuthType
(
	id INT PRIMARY KEY AUTO_INCREMENT, 
    identity_type NVARCHAR(128) NOT NULL UNIQUE
);

#插入基础授权类型
INSERT INTO AuthType (id,identity_type) VALUES (1,"Account");
INSERT INTO AuthType (id,identity_type) VALUES (2,"PhoneNumber");
INSERT INTO AuthType (id,identity_type) VALUES (3,"Email");
INSERT INTO AuthType (id,identity_type) VALUES (4,"WeChat");