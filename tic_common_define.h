#ifndef TIC_COMMON_DEFINE_H
#define TIC_COMMON_DEFINE_H


/*-------------------------------------------------String------------------------------------------*/
//ADDRUPDATE_DIALOG
#define STR_ADDRNAME_IS_REGISTERED            "该地址名已被注册"
#define STR_ADDRNAME_IS_EXIST                 "钱包里已经存在这个地址名"
#define STR_ADDRNAME_IS_TOO_LONG              "地址名太长，请输入63位以内的地址名"
#define STR_ADDRNAME_MUST_BEGIN_BY_LOWER      "首字符(包括以'.'分隔的单词的首字符)必须是小写字母"
#define STR_ADDRNAME_MUST_END_BY_LOWER_OR_NUM "尾字符必须是小写字母或者数字"
#define STR_ADDRNAME_HAVE_UPPER_OR_INVALID    "地址名包含了大写字母或者特殊字符"
#define STR_ADDRNAME_IS_VALID                 "该账户可用"
#define STR_NEED_TRANSFER_FROM_OFFICIAL       ""
#define STR_TRANSFER_TO_PAY                   "转账支付"
#define STR_ADDRNAME_INFO                     "只允许小写字母，点和横杠。必须为小写字母开头，结尾不能为横杠"
#define STR_ADDRUPDATE_PAY_INFO               "需支付0.01GOP进行升级"
#define STR_ADDRUPDATE_SUBMITED               "升级申请已提交"
#define STR_CONFIRM                           "确认"

//APPLY_DELEGATE_DIALOG
#define STR_APPLY_DELEGATE_PAY_INFO            "申请成为代理所需费用：1221.83 GOP"
#define STR_APPLY_DELETE                       "申请代理："
#define STR_NEXT                               "下一步"
#define STR_PAY                                "支付"



/*-------------------------------------------------Int&Float------------------------------------------*/
#define GOP_BLOCKCHAIN_NAME_MAX_SIZE           63
#define BLOCKCHAIN_TRANSFER_MIN_FEE            0.01
#define BLOCKCHAIN_ADDRESS_LENGTH              68
#define BLOCKCHAIN_GET_ACCOUNT_RES_LENGTH      16
#define BLOCKCHAIN_RENAME_RES_LENGTH           2
#define RES_CODE_LENGTH                        5
#define ADDRNAME_MAX_LENGTH                    63
#define ADDRNAME_MIN_LENGTH                    1


/*-------------------------------------------------Result------------------------------------------*/
#define RES_CODE_UNKNOW_WALLET_ACCOUNT      "20006"                     //钱包里没有该账户
#define RES_CODE_UNKNOW_BLOCK_CHAIN_ACCOUNT "No account found"          //该账户未注册
#define RES_CODE_UNAUTHORIZED_CHILD_ACCOUNT "20039"                     //父账户没有注册

#endif // TIC_COMMON_DEFINE_H

