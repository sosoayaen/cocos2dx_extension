/**
* @author Jason Tou
* @date 20130723
* @comment 本类是基于DialogLayer的引导层，可以用于新手引导
* 1. 使用create创建层
* 2. 创建GUIDING_STEP引导节点
*  2.1 设置对应的触摸响应控件。 pTouchedCtl 
*  2.2 设置对应的ENUM_CTRL_TYPE类型。 nCtrlType
*  2.4 设置新手引导层高亮的光圈大小，计算的是给出控件矩形对角线大小为光圈直径。 pRectCtl
*      如果此控件未提供，则以触摸响应控件的大小为基准。
*  2.5 设置新手引导节点对应的额外显示控件，需要在外部retain。pTipsNode
*  2.6 设置额外控件的动作，需要在外部retain。 pTipsAction
*      这里只对整个pTipsNode执行runAction，pTipsNode的内部动作不能通过这里设置
*  2.7 设置当前步骤是否可以点击屏幕任意区域继续。 bClickOutside， 默认为false
* 3. 使用AppendGuidingStep函数将引导节点依次添加到队列中。这里的入队列顺序决定了新手引导的
*    播放过程，目前暂不支持回退，PreviouseAction是空函数
* 4. 重复2和3直到添加完毕后，调用InitCurrIterator函数，使引导指针回到第一个的位置
* 5. 添加当前层到你所需要新手引导的层中
* 6. 调用NextAction()使得新手引导一步一步向下，最后结束后会移除该层
*/

#ifndef __GUIDING_LAYER_h__
#define __GUIDING_LAYER_h__

#include "DialogLayer.h"

enum ENUM_CTRL_TYPE
{
	TOUCH_CTRL_TYPE_NULL = 0,
	TOUCH_CTRL_TYPE_MENU,			// CCMenu
	TOUCH_CTRL_TYPE_EDITBOX,		// CCEditBox
	TOUCH_CTRL_TYPE_SCROLLVIEW,		// CCScrollView
	TOUCH_CTRL_TYPE_OTHERCTL		// Other
};
/*
* @brief 引导动作结构体
*
*/
typedef struct tagGuidingStep
{
	/* 触摸响应操作的控件 */
	cocos2d::CCNode* pTouchedCtl;
	/* 控件类型 */
	int nCtrlType;
	/* 控制大小的控件 */
	cocos2d::CCNode* pRectCtl;
	/* 对应显示到屏幕上的提示对象，坐标是全屏坐标 */
	cocos2d::CCNode* pTipsNode;
	/* 提示文字的动作 */
	cocos2d::CCActionInterval* pTipsAction;
	/* 点击可执行下一步的区域 */
	cocos2d::CCRect clickRect;
	/* 是否点击区域外执行下一步 */
	bool bClickOutside;

	tagGuidingStep()
	{
		Clear();
	}

	void Clear()
	{
		pTouchedCtl = NULL;
		pRectCtl = NULL;
		pTipsNode = NULL;
		pTipsAction = NULL;
		clickRect.setRect(0, 0, 0, 0);
		nCtrlType = TOUCH_CTRL_TYPE_NULL;
		bClickOutside = false;
	}

} GUIDING_STEP;

class GuidingLayer : public DialogLayer
{
public:
	virtual ~GuidingLayer();

	CREATE_FUNC(GuidingLayer);

	virtual bool onInitDialog();

	virtual bool init();

	virtual void onEnter();

	virtual void onExit();

public:
	/**
	* @brief 返回一个全局静态的新手引导管理对象
	*
	*/
	static GuidingLayer* sharedGuidingLayer();

	/**
	* @brief 在队列末尾添加引导步骤
	*
	*/
	void AppendGuidingStep(const GUIDING_STEP& gd_step);

	/**
	* @brief 下一步操作
	*
	*/
	void NextStep();

	/**
	* @brief 跳过一个操作
	*/
	void JumpNextStep();

	/**
	* @brief 上一步操作
	*
	*/
	void PreviousStep();

	/**
	* @brief 初始当前迭代器的位置
	*
	*/
	void InitCurrIterator();

	/**
	* @brief 是否启用新手引导功能
	* @param 是否启用
	*
	*/
	static void EnableGuide(bool bEnable);

	/**
	* @brief 设置遮罩层的透明度
	*/
	void SetMaskOpacity(GLubyte opacity);

	/**
	* @brief 设置遮罩层颜色
	*/
	void SetMaskColor3B(cocos2d::ccColor3B color);

	/**
	* @brief 设置高亮光圈移动的时间
	*/
	void SetHightLightMoveTime(float fTime);

	/**
	* @brief 单步操作，释放屏幕捕捉监听，以及使得高亮消失
	* 当新手引导动作非连续的状态时，可以单步执行调用该接口
	* 暂时停止界面的触摸捕捉和高亮效果
	*/
	void ReleaseFocus();

	/**
	* @brief 重新添加监听
	*/
	void ResetFocus();

	void ActionCallbackRemoveSelf();
private:
	/**
	* @brief 在当前位置添加引导步骤
	*
	*/
	void InsertGuidingStep(const GUIDING_STEP& gd_step);

	virtual bool ccTouchBegan( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent );


protected:
	/* 当前位置 */
	std::list<GUIDING_STEP>::iterator m_iterCurrent;

	int m_nPos;

	/* 引导的内容队列（数组） */
	std::list<GUIDING_STEP> m_listGuidingStep;

	/* 裁剪对象 */
	cocos2d::CCClippingNode* m_pClipper;

	/*
	放置裁剪对象的模板层
	所有的可触摸控件都在这层上
	*/
	cocos2d::CCNode* m_pStencilLayer;

	/* 颜色遮罩 */
	cocos2d::CCLayerColor* m_pMaskLayer;

	/* 高亮圆圈 */
	cocos2d::CCSprite* m_pHole;

	/* 是否启用 */
	static bool m_bEnable;

	/* 高亮光圈移动的时间 可以在调用NextAction前动态指定*/
	float m_fMotionTime;

	/* 是否失去焦点 */
	bool m_bFocused;
};

#endif // __GUIDING_LAYER_h__