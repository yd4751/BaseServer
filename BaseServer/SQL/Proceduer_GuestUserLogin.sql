USE bighat;

DROP PROCEDURE IF EXISTS GuestUserLogin;

DELIMITER //
CREATE PROCEDURE GuestUserLogin (OUT retCode INT,OUT retUserID INT)
LABEL_RUN:
BEGIN
	#游客登录 默认配置
    DECLARE getUserID INT DEFAULT 0;
	DECLARE identify_id INT;
    DECLARE identify NVARCHAR(256);
    DECLARE credential NVARCHAR(256);
    
    SET identify_id = (select id from AuthType where identity_type = "Account");
    SET credential = "12345678";
 
	#分配UID
    insert into UserInfo (nick_name,avatar,update_time) values ("","www.google.com",now());
    SET getUserID = (select max(user_id) from UserInfo);
    set identify = CONCAT('guest_',getUserID);

	#赠送游客1000币
	UPDATE UserInfo SET nick_name = CONCAT('游客',getUserID) , money = 1000 WHERE user_id = getUserID;
    
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