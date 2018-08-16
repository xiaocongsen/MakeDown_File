# 存储过程

## 代码例子:
``` sql

CREATE DEFINER=`root`@`localhost` PROCEDURE `RepealOrder`(
IN order_id varchar(36),
IN ncounter_id varchar(36),
IN nuser_id varchar(36),
IN ncoin_id varchar(36),
IN nmoney DECIMAL(20,8),
IN nstatus int,
OUT sum int)
BEGIN
		DECLARE Count INT;
		UPDATE T_BB_ORDER SET fstatus = nstatus, fcomplete_time = now() 
			WHERE forder_id = order_id AND (fstatus=1 or fstatus=2);
		SET Count = ROW_COUNT();
		IF Count = 1 THEN
				UPDATE T_USER_ASSET_BB SET fcount = fcount+nmoney, ffrozen = ffrozen-nmoney, version = version+1 
				where fcounter_id = ncounter_id AND fuser_id = nuser_id AND fcoin_id = ncoin_id;
		END IF;
		SET sum = Count;
END
--------------------------------------
CREATE DEFINER=`root`@`localhost` PROCEDURE `EntrustProcess`(
IN ntaker_order_id varchar(36), 
IN nmaker_order_id varchar(36),
-- -- 订单
-- IN ncounter_id varchar(36),
-- -- 柜台id
-- IN nmarket_id varchar(36),
-- -- 主币id
-- IN ncoin_id varchar(36), 
-- -- 副币id
-- IN ntaker_user_id varchar(36),				
-- IN nmaker_user_id varchar(36),
-- 用户id
IN ntaker_get_count DECIMAL(20,8),
IN nmaker_get_count DECIMAL(20,8),
-- 得到币的数量
IN ntaker_fee DECIMAL(20,8),					
IN nmaker_fee DECIMAL(20,8),
-- 得到币的手续费
IN ntaker_remain DECIMAL(20,8),				
IN nmaker_remain DECIMAL(20,8),
-- 买卖中剩余的副币数量
IN ntaker_deal DECIMAL(20,8),				
IN nmaker_deal DECIMAL(20,8),
-- 累计金额
IN nprice DECIMAL(20,8), 
-- IN ntaker_side INT,			
-- IN nmaker_side INT,
-- 买卖类型
IN nmaker_status INT,
IN ntaker_status INT,
-- 状态
OUT sum INT)
BEGIN
		DECLARE Count INT;
-- 		DECLARE time TIMESTAMP;
-- 可能需要查一下是否有2个订单存在
		SELECT count(fstatus) INTO Count FROM T_BB_ORDER WHERE forder_id = ntaker_order_id or forder_id = nmaker_order_id;
		IF Count = 2 THEN
			UPDATE T_BB_ORDER SET fstatus = ntaker_status,fremain = ntaker_remain, fdeal = ntaker_deal,fcomplete_time = now() 
			WHERE forder_id = ntaker_order_id;
			UPDATE T_BB_ORDER SET fstatus = nmaker_status,fremain = nmaker_remain, fdeal = nmaker_deal,fcomplete_time = now() 
			WHERE forder_id = nmaker_order_id;

			INSERT into T_BB_EXCHANGE values(UUID(),nmaker_order_id,ntaker_order_id,ntaker_get_count,nmaker_get_count,nprice,nmaker_fee,ntaker_fee,now());

			IF ntaker_side = 2 THEN
	-- 		买 失去主币 得到副币 
						UPDATE T_USER_ASSET_BB SET ffrozen = ffrozen - ntaker_get_count * nprice, version = version + 1 
						where fcounter_id = ncounter_id AND fuser_id = ntaker_user_id AND fcoin_id = nmarket_id;
						
						UPDATE T_USER_ASSET_BB SET fcount = fcount + ntaker_get_count, version = version+1 
						where fcounter_id = ncounter_id AND fuser_id = ntaker_user_id AND fcoin_id = ncoin_id;
	-- 		对方 得到主币 失去副币
						UPDATE T_USER_ASSET_BB SET ffrozen = ffrozen + ntaker_get_count * nprice, version = version + 1 
						where fcounter_id = ncounter_id AND fuser_id = nmaker_user_id AND fcoin_id = nmarket_id;
						
						UPDATE T_USER_ASSET_BB SET fcount = fcount - ntaker_get_count, version = version+1 
						where fcounter_id = ncounter_id AND fuser_id = nmaker_user_id AND fcoin_id = ncoin_id;
				ELSE
	-- 		卖 得到主币 失去副币
						UPDATE T_USER_ASSET_BB SET ffrozen = ffrozen + ntaker_get_count, version = version + 1 
						where fcounter_id = ncounter_id AND fuser_id = ntaker_user_id AND fcoin_id = nmarket_id;
						
						UPDATE T_USER_ASSET_BB SET fcount = fcount - ntaker_get_count / nprice, version = version+1 
						where fcounter_id = ncounter_id AND fuser_id = ntaker_user_id AND fcoin_id = ncoin_id;
	-- 		对方 失去主币 得到副币
						UPDATE T_USER_ASSET_BB SET ffrozen = ffrozen - ntaker_get_count, version = version + 1 
						where fcounter_id = ncounter_id AND fuser_id = nmaker_user_id AND fcoin_id = nmarket_id;
						
						UPDATE T_USER_ASSET_BB SET fcount = fcount + ntaker_get_count / nprice, version = version+1 
						where fcounter_id = ncounter_id AND fuser_id = nmaker_user_id AND fcoin_id = ncoin_id;
				END IF;
		END IF;
			SET sum = Count;
END
```
## 执行语句

``` sql
    call EntrustProcess("20150522151102",220.0235,101.2456,@s);
    select @s as sum;
    select * from T_BB_ORDER where forder_id = "20150522151114";

    call EntrustProcess("2015052212425","20150522150753",'1','USDT','BTC','xiaocs','admin',100,50,0,0,0,0,18800,8300,0.5,1,2,3,3,@s);

    call EntrustProcess("2015052212425","20150522150753",100,50,0,0,0,0,18800,8300,0.5,3,3,@s);
    CALL EntrustProcess('2015052212425','20150522150753',100.1,50.1,0.1,0.2,0.3,0.4,18800.4,8300.7,0.000000005,3,3,@pop)
```
