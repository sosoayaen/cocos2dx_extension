// 原理：
// 模态对话框 能够拦截所有下层的消息
// cocos2d-x 消息处理优先级【-128,128】越小优先级越高
// 菜单kCCMenuHandlerPriority优先级 -128 要想拦截所有消息(包括菜单) 
// 必须将我们的模态对话框优先级<-128 但是也意味着拦截了当前模态对话框的菜单消息(-128)
// 我们需要手动将消息传给模态对话框上的按钮进行处理
// 使用：1.重载OnInitDialog将需要的控件添加到dlg上
//       2.需要响应事件的按钮使用pushMenu添加
//       3.派生类需要实现宏CREATE_FUNC(class_name)

#ifndef _DIALOGLAYER_H_
#define _DIALOGLAYER_H_

#include "cocos2d.h"
#include "cocos-ext.h"

class DialogLayer: public cocos2d::CCLayerColor
{
public:
    DialogLayer();
    virtual ~DialogLayer();

public:
    /** 初始化对话框 */
    virtual bool onInitDialog() = 0;

    /**
	所有的菜单按钮压入到容器中
	当前函数必须在 onInitDialog 中调用
	*/
	void pushMenuItem(cocos2d::CCMenuItem* pMenuItem);

	/** 
	设置多个可响应的按钮，一般用在ccbi载入后获取对应的菜单，使其可响应界面消息
	当前函数可以在任意时刻调用
	*/
	void pushMenu(cocos2d::CCMenu* pMenu);

	/**
	*@brief 从事件响应列表中移除按钮的触摸响应
	*/
	bool popMenu(cocos2d::CCMenu* pMenu);

	/**
	*@brief 从事件响应列表中移除按钮的触摸响应
	*/
	bool popMenuByTag(int nTag);

	void pushEditBox(cocos2d::extension::CCEditBox* pEditBox);

	/**
	*@brief 从事件响应列表中移除按钮的触摸响应
	*/
	bool popEditBox(cocos2d::extension::CCEditBox* pEditBox);

	/**
	*@brief 从事件响应列表中移除按钮的触摸响应
	*/
	bool popEditBoxByTag(int nTag);

	void pushScrollView(cocos2d::extension::CCScrollView* pScrollView);

	/**
	*@brief 从事件响应列表中移除按钮的触摸响应
	*/
	bool popScrollView(cocos2d::extension::CCScrollView* pScrollView);

	/**
	*@brief 从事件响应列表中移除按钮的触摸响应
	*/
	bool popScrollViewByTag(int nTag);

	// 其余一些可以接收触摸消息的控件
	void pushOtherTouchedCtl(cocos2d::CCLayer* pOtherCtl);

	bool popOtherTouchedCtl(cocos2d::CCLayer* pOtherCtl);

	bool popOtherTouchedCtrlByTag(int nTag);

    virtual bool init();

    virtual void onEnter();

	/*
	这里一定要自己调用对应的注销触摸的事件
	虽然在CCLayer中已经有对应的设置，但是
	需要通过设置setTouchEnalbe实现，并且
	设置注册对应消息时是默认优先级，这样就
	达不到模态对话框的效果，导致后面的按钮
	还是会被触发
	*/
    virtual void onExit();

    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);


	/**
	* @brief 设置可视区域，当设置此值后，点击此区域外会移除当前Dialog，调用endDialog函数
	*        需要注意的是，如果使用addBoardChildWithAction添加主面板，会同步设置该主面板
	*        为可视面板，但是如果不希望点击周边关闭对话框，则可以调用此函数，传入NULL值
	* @param pNode 可视节点，要求至少为CCSprite，因为要得到ContentSize来确定可视区域
	*/
	void SetVisibleBoard(cocos2d::CCNode* pNode);

	/**
	* @brief 在结束动画之前，把面板的颜色置为透明
	*/
	void SetBackgroundColorOffBeforeEndDialogAction(bool bOff);

	/**
	* @brief 结束对话框函数，如果设置了出场动画，则播放后结束，如果设置了动画结束回调则会调用对应的回调函数
	* @param pAction 如果提供此参数，则修改出场动画效果，否则默认使用addBoardChildWithAction设定时的pOnExitAction动画
	* @param pDuration 表示动画时间，暂时不使用
	* @return true 默认返回成功
	*/
	bool endDialog(cocos2d::CCActionInterval* pAction = NULL, float fDuration = 1.0f);
	
	/**
	* @brief 添加整体对话框，附加开始和结束动画
	* @param pChildNode 设为主面板的节点，默认也设置为可视面板，如果主面板只是负责移动的CCNode容器，则需要指定可视面板。@see SetVisibleBoard
	* @param pOnEnterAction 进场时主面板的动作，可以为NULL
	* @param pOnExitAction 移除Dialog时的动作，可以为NULL
	* @param pAfterExitActionCallbackFuncN 移除Dialog动作播放结束后的回调函数（可以外部得到一个操作的机会），可以为空
	* @return true 默认返回成功
	*/
	bool addBoardChildWithAction(cocos2d::CCNode* pChildNode, cocos2d::CCActionInterval* pOnEnterAction = NULL, cocos2d::CCActionInterval* pOnExitAction = NULL, cocos2d::CCCallFuncN* pAfterExitActionCallbackFuncN = NULL);


private:
    /** 初始化菜单 将菜单添加到模态对话框上*/
    bool initMenu();

	// 移除DialogLayer
	void ActionCallbackRemoveLayer();

protected:            // 模态对话框菜单
    cocos2d::CCArray *mMenuItemArray;   // 所有菜单
//    bool mTouchedMenu;                  // 记录菜单点击

	/** 当前被选中的按钮 CCMenu */
	cocos2d::CCMenu* m_pCurrentTouchMenu;

	cocos2d::extension::CCEditBox* m_pCurrentTouchEditBox;

	cocos2d::extension::CCScrollView* m_pCurrentTouchScrollView;

	// 一些简单的可以响应点击事件的控件（暂时不支持多点）
	cocos2d::CCLayer* m_pOtherTouchCtl;

	// 当触摸点数，begin + 1， end - 1
	int m_nTouchCount;
	/**
	存放界面上所有的Menu，用以循环判断是否有按下，如果有多个层次
	则注意要及时设置下层不可见的按钮状态为disable，否则可能会造成误响应
	*/
	std::vector<cocos2d::CCMenu*> m_vMenuArrays;

	std::vector<cocos2d::extension::CCEditBox*> m_vEditBoxArrays;

	std::vector<cocos2d::extension::CCScrollView*> m_vScrollViewArrays;

	std::vector<cocos2d::CCLayer*> m_vOtherCtlArray;

	// 整体显示面板，所有的按钮控件都在这层面板区域内，用来判断是否点击到空白区域作为退出的凭证
	cocos2d::CCNode* m_pVisibleNode;

	// 整体面板，要求只能有一个根面板存在，它控制整个对话框的动作
	cocos2d::CCNode* m_pMainBoard;

	// 整体面板进场动画
	cocos2d::CCActionInterval* m_pBoardOnEnterAction;

	// 整体面板出场动画
	cocos2d::CCActionInterval* m_pBoardOnExitAction;

	// 对应点击到退出区域的响应，需要由外部控制调用移除当前Dialog
	cocos2d::CCCallFuncN* m_pCallFuncN;

	// 在结束动画之前把背景默认颜色移除为透明
	bool m_bOffBackgoundColorBeforeEndDialogAction;
};

#endif