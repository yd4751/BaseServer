USE bighat;

DROP PROCEDURE IF EXISTS UserLogin;

DELIMITER //
CREATE PROCEDURE UserLogin (IN identify_id INT, IN identify NVARCHAR(256),IN credential NVARCHAR(256),OUT retCode INT,OUT retUserID INT)
LABEL_RUN:
BEGIN
	#判断有效授权类型CreateUser
    if not exists (select * from AuthType where id = identify_id)
    then
		set retCode = -1;
        leave LABEL_RUN;
    end if;
    
	#判断授权存在
    if not exists (select * from UserAuthInfo where identity_id = identify_id and identifier = identify)
    then 
		set retCode = -2;
        leave LABEL_RUN;
    end if;
    
    #校验密码凭证
    if not exists (select * from UserAuthInfo where identifier = identify and credential = credential)
    then
		set retCode = -3;
        leave LABEL_RUN;
    end if;
    
    set retCode = 0;
    set retUserID = (select user_id from UserAuthInfo where identifier = identify);
    
END LABEL_RUN;
//
DELIMITER ;