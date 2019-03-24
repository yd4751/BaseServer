USE bighat;

DROP PROCEDURE IF EXISTS CreateUser;

DELIMITER //
CREATE PROCEDURE CreateUser (IN identify_id INT, IN identify NVARCHAR(256),IN credential NVARCHAR(256),IN nick_name NVARCHAR(256),OUT retCode INT,OUT retUserID INT)
LABEL_RUN:
BEGIN
	DECLARE getUserID INT DEFAULT 0;
	
    
    #判断有效授权类型
    if not exists (select * from AuthType where id = identify_id)
    then
		set retCode = -1;
        leave LABEL_RUN;
    end if;

	#判断授权存在
    if exists (select * from UserAuthInfo where identity_id = identify_id and identifier = identify)
    then 
		set retCode = -2;
        leave LABEL_RUN;
    end if;
    
    #分配UID
    insert into UserInfo (nick_name,avatar,update_time) values (nick_name,"www.google.com",now());
    set getUserID = (select max(user_id) from UserInfo);

	#加入授权信息
    insert into UserAuthInfo (
    	user_id,
		identity_id,
		identifier,
		credential,
		verfiy
	) values (
		getUserID,
        identify_id,
        identify,
        credential,
        0
    );
    
    set retCode = 0;
    set retUserID = getUserID;
    
END LABEL_RUN;
// 
DELIMITER ;
