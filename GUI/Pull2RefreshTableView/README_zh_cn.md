#Pull2RefreshTableView

----------

#版本信息
|Date     |version|
|---------|-------|
|2014/1/29|1.0.0  |


#简介
`Pull2RefreshTableView`是适用于`cocos2dx`的`2.x`系列。目前我是在`2.1.3`这个版本上进行开发的，至少在`2.2.2`中也可以正常运行。因为是直接在引擎层上的二次开发，所以原则上支持所有`cocos2dx`支持的目标平台，无需针对比如`iOS`或者`Android`做`native`的代码设置。

它继承自`cocos2d::extension::CCTableView`，提供一个相对简易的接口来实现一个**下拉释放刷新**的功能，类似于[twitter][1]的下拉刷新的实现。当然，它还提供**上拉载入更多**的接口，因为原理其实是一样的。

如果要使用本控件，有一点很不爽的地方是你需要在`cocos2d::extension::CCScrollView`类中增加一个`protected`的函数，以保证编译能够通过。一劳永逸的办法当然是能把这个控件加入到官方的扩展库中，不过我暂时没有这个时间去做这件事情，如果有人能把它变成官方引擎的一部分，我会很感谢他。

#如何使用
如果你会用`cocos2d::extension::CCTableView`，那恭喜你，你将会很快上手这个控件，如果不会使用也没有关系，我这里也会做一个简单的介绍。

##修改基类
找到`cocos2d::extension::CCScrollView`的头文件，在`pretected`下增加以下代码：
```
protected:
	/**
	 * @brief stop the scroll animation, in additional, it will stop the schedule whitch affected the animation
	 */
	void stopScrollAnimation();
```

在对应的`CPP`中加上实现：
```
void CCScrollView::stopScrollAnimation()
{
	// stop container's animation
	m_pContainer->stopAllActions();

	// stop the schedule
    this->unschedule(schedule_selector(CCScrollView::performedAnimatedScroll));
    this->unschedule(schedule_selector(CCScrollView::deaccelerateScrolling));
}
```
###修改CCScrollView的基类的原因
拿[新浪微博][2]做个例子，当你在拖动列表下拉时，顶部会出现一个提示，并且当你达到它的刷新条件（拖到一定位置后放开），则整个列表不会马上恢复初始位置，而是在顶部显示一个动画，比如是载入的滚动的菊花之类的；在`CCScrollView`中，本身设定了当表被向下拖动到范围之外时，会自动回正，主要就是`CCScrollView::deaccelerateScrolling`这个定时器回调实现的。为了实现在顶部空出一个动画的空间，子类需要停止view的滚动，手动指定到一个偏移的位置。但是那些停止定时器和动画效果的函数都是私有的，所以目前使用的方案是单独写一个`stopScrollAnimation`的接口供子类调用，从整体性考虑，把此函数定义为`protected`域，不允许外部调用。

这样带来的问题就是引擎升级以后如果还要使用此类的功能，就必须同步修改对应的基类。

##加入项目
控件就一个头文件和一个源文件，分别是`Pull2RefreshTableView.h`和`Pull2RefreshTableView.cpp`，只需要把这两个文件放到项目中即可，至于什么位置无所谓，只要你的项目能找到并且编译就好。

##创建并设置表格参数
###create函数
```
/**
 * @brief 创建表格
 * @param pSource 和CCTableView一样，需要设置一个数据源处理对象，它会处理一些数据刷新之类的回调
 * @param size 表格的可视区域大小
 * @param pContainer 放置表格内容的层，一般设置为空即可
 * @param pHeaderNode 顶部的控件容器，默认其高度为顶部下拉载入时的偏移量
 * @param pFooterNode 底部的控件容器，默认高度为底部上拉载入时的偏移量
 * @return Pull2RefreshTableView
 */
static Pull2RefreshTableView* create(CCTableViewDataSource* pSource, CCSize size, CCNode* pContainer = NULL, CCNode* pHeaderNode = NULL, CCNode* pFooterNode = NULL);
```

创建表格需要设置数据源、可视大小、表格操作回调宿主（Delegate）以及顶部的节点和底部节点。由于各个下拉刷新列表可以自定义一些风格，所以建议顶部和底部的内部控件最好在宿主对象中有对应的成员变量可供方便调用。

>例如，可以在外部创建一个`m_pSpriteHeaderLoading`的圈圈`Sprite`图片，添加到一个节点中，然后把这个节点当*下拉刷新*事件被触发通知时，外部设置`m_pSpriteHeaderLoading->runAction(CCRepeatForever::create(CCRotateBy(0.5f, 360))`，使得这个圈圈的图片以每0.5秒1圈的速度旋转。当数据刷新回来后，设置圈圈移除所有的动作`m_pSpriteHeaderLoading->stopAllActions()`。

创建一个表格非常简单，可以如下代码所示
```
Pull2RefreshTableview* table = Pull2RefreshTableView::create(this, CCSizeMake(100, 300));
```
好了，这样就创建起一个简单的下拉刷新表格对象`table`了，可视大小是宽100像素，高300像素。但是这样创建出来的表对象和他的基类`CCTableView`毫无二致，所以，我们还是需要配置一下，以便它看上去像个下拉刷新表格。

###设置操作接口
```
table->setDelegate(this);
```
把当前的外部操作类设置为宿主即可，参见[设定表格控件的回调实现](#设定表格控件的回调实现)。

宿主（玄乎了点）需要实现一些固定的回调，参见[Pull2RefreshTableView的回调接口](#pullrefreshtableview的回调接口)。

###设置顶部节点
既然是下拉刷新，那么肯定会有一个展现载入的节点，使用`Pull2RefreshTableView::setHeaderNode(CCNode* pNode)`函数即可增加一个节点到表格中预留好的头部展现区域；同样的，也有`Pull2RefreshTableView::setFooterNode(CCNode* pNode)`存在，用来设置上拉刷新的展现。

```
// 创建顶部节点容器
CCNode* pHeaderNode = CCNode::create();
// 创建一个用户刷新等待的图片精灵，为了访问方便，此处为外部操作类的成员变量
m_pSpriteHeaderLoading = CCSprite::create("headerLoading.png");
// 设置顶部按钮的高度为精灵的高度
pHeaderNode->setContentSize(CCSizeMake(100, m_pSpriteHeaderLoading->getContentSize().height));
// 设置加载图片居中放置
m_pSpriteHeaderLoading->setPosition(ccpMult(ccpFromSize(pHeaderNode.getContentSize()), 0.5f));
// 添加到顶部节点容器
pHeaderNode->addChild(m_pSpriteHeaderLoading);
// 把节点设置到表中
table->setHeaderNode(m_pSpriteHeaderLoading);
```

在`create`函数中也可以直接设置这个HeaderNode。

```
Pull2RefreshTableview* table = Pull2RefreshTableView::create(
    this,
    CCSizeMake(100, 300),
    NULL,   // Container的对象，不传入的话表格自己会创建
    pHeaderNode);
```

###设置触发阙值

我们还需要设置一个距离来告诉表格，下拉偏移多少距离就触发下拉刷新（同样，上拉刷新也需要设置对应的数值），如果不设置，那么你必须给头部或者底部的节点设置高度，因为表格默认会取这个高度作为触发的距离条件。

>举例来讲，比如一个表格可视区域高100像素，然后头部载入的节点高20像素，那么当你向下拉表格距离超过20像素时，就会触发*下拉刷新回调*。同样，如果你设置了Footer节点的高度，则上提加载更多也会在适当的距离后被触发。

通过函数`void Pull2RefreshTableView::setPullDownThreshold(float fThreshold);`设置下拉阙值，`void Pull2RefreshTableView::setPullUpThreshold(float fThreshold);`设置上拉阙值。

###设置触发偏移量阙值
除此之外，我们还可以为这个拉动的距离设定一个偏移量，如果偏移量是正的，则拖动举例会变长，而如果偏移量是负的，则会减少引起触发的距离。

>举例来讲，按照上面的表格设定，100像素高，头部载入的节点高20像素，然后再设定头部拉动偏移量为10像素，则当你向下拉动表格时，到10像素的距离，才刚触发下拉开始的回调，然后直到拉动超过30像素才会触发刷新回调。如果偏移量是-10，那么只要往下拉动就直接回触发*下拉回调*，而向下拉10个像素就会触发下拉刷新回调。*上拉操作*也是同样。一般不需要设置这个偏移参数，默认为0。

通过函数`void Pull2RefreshTableView::setPullDownOffsetThreshold(float fThreshold);`设置下拉偏移量阙值，`void Pull2RefreshTableView::setPullUpOffsetThreshold(float fThreshold);`设置上拉偏移量阙值。

##设定表格控件的回调实现
设定一个`Delegate`的继承类，它用来响应一些表格的状态修改和一些通知回调等。如果会使用`CCTableView`的话，那我要说，用法是一样的，只是多了几个必须实现的函数罢了。一般会使用当前展现的场景类来继承并实现响应的回调逻辑。

##Pull2RefreshTableView的回调接口

`Pull2RefreshTableView.h`头文件中定义了一个代理类：`Pull2RefreshTableViewDelegate`，它继承自`cocos2d::extension::CCTableViewDelegate`，定义了响应`Pull2RefreshTableView`的回调接口。一般是由包含表格的容器来继承这个`Pull2RefreshTableViewDelegate`，然后实现虚函数，在创建表格的时候使用`Pull2RefreshTableView::setDelegate(Pull2RefreshTableViewDelegate* pDelegate)`把这个类的实例注册到TableView中，以实现相应的回调。

一共有6个接口，其中4个是纯虚的函数，需要继承的子类实现对应的业务。

```
 public:
    /**
     * 这两个函数在列表触发下拉或者上拉刷新条件后触发，一般用于外部加载更新数据操作
     */
	virtual bool onPullDownRefresh(Pull2RefreshTableView* table) = 0;
	virtual bool onPullUpRefresh(Pull2RefreshTableView* table) = 0;

	/**
	 * 当外部的数据加载完成后，需要主动调用下这两个函数，内部默认会刷新列表，并且把
	 * 列表恢复到正常的位置
	 */
	virtual void onPullDownRefreshComplete(Pull2RefreshTableView* table);
	virtual void onPullUpRefreshComplete(Pull2RefreshTableView* table);

	/**
     * 当在列表上进行下拉操作时会回调这个函数。因为调用会很频繁，所以不要在这个函数
     * 体中进行耗时的操作。在回调过程中可以外部改变对应的控件来实现复杂的效果
     * 参数 table 表示下拉操作对应的表对象
     * 参数 fDistance 为对应拉操作的位移值
     * 参数 fThreshold 为触发刷新拉操作移动阙值
	 */
	virtual void onPullDownDidScroll(Pull2RefreshTableView* table, float fDistance, float fThreshold) = 0;

	/**
     * 当在列表上进行上拉操作时会回调这个函数。因为调用会很频繁，所以不要在这个函数
     * 体中进行耗时的操作
     * 参数 table 表示下拉操作对应的表对象
     * 参数 fDistance 为对应拉操作的位移值
     * 参数 fThreshold 为触发刷新拉操作移动阙值
	 */
	virtual void onPullUpDidScroll(Pull2RefreshTableView* table, float fDistance, float fThreshold) = 0;
```

##如何实现拉动时的动画
拿下拉动画来讲，当对表格进行下拉操作后，继承`Pull2RefreshTableViewDelegate`并`setDelegate`注册对应实例到对应table中的实例会触发`onPullDownDidScroll`回调，其实这个回调函数可以看作是动画帧的回调。

`onPullDownDidScroll`函数有三个参数：

1. Pull2RefreshTableView* table
* 对应发生下拉拖动的表格实例指针
2. float fDistance
* 在当前表格上拖动的距离
3. float fThreshold
* 当前表格设置的触发回调的距离阙值，如果在这个距离上释放会触发`onPullDownRefresh`回调

动画帧的回调其实只需要根据当前的数据设置一个状态即可，比如设定了拖动距离为40像素就触发数据刷新回调，并且为简单起见，设定偏移量为0，也就是默认不调用`setPullDownOffsetThreshold`函数。那么，当表格被向下拖动了20像素时，`onPullDownDidScroll`回调中的第二个参数值为`20`，第三个参数值为`40`，表示拖动了一半的阙值，如果用一个旋转的菊花图片作为动画对象，并且设定从拖动到触发会旋转180度，那么可以通过公式`float degree = 180*fDistance/fThreshold;`得出当前转动的角度，直接通过`CCNode`的`setRotation(degree)`设置精灵的旋转角度，这一帧动画就设置好了。拖动时会多次触发回调，当这些帧被快速替换更新后，动画效果就出现了。

>在`onPullDownDidScroll`回调中，请不要调用`runAction`或者是增加定时器之类的操作，因为它被触发的频率非常高，可以把它看成是一个判断触点moved的消息回调，只要手指移动了，都会触发，所以当你在屏幕上拉动表格时，会触发很多次`onPullDownDidScroll`。

##刷新数据
数据刷新的操作只需要在`Pull2RefreshTableViewDelegate::onPullDownRefresh`和`Pull2RefreshTableViewDelegate::onPullUpRefresh`回调中实现即可。例如，你可以使用`http`请求一些`JSON`数据回来，然后当数据返回后主动调用`Pull2RefreshTableView::onPullDownRefreshComplete`或者`Pull2RefreshTableView::onPullDownRefreshComplete`函数通知表格数据获取接触，刷新表格。

`Pull2RefreshTableViewDelegate`也提供了一个简单版本的`onPullDownRefreshComplete`和`onPullUpRefreshComplete`，不过都带一个`Pull2RefreshTableView`对象指针参数，也就是说当你做简单调用的时候，只需要把数据刷新回来的对象放到宿主调用的`onPullXXXRefreshComplete(table)`即可。和`CCScrollView`一样，宿主可以同时拥有多个表，所以需要知道你通知数据刷新完成的表是哪个。

###数据超时的处理
对于数据超时，其实不在表的控制范围内，不过这里也顺道提一下。最好代码中能有数据超时的处理，一旦超时后，请主动调用`Pull2RefreshTableView::onPullXXXComplete()`系列函数，或者是调用`Pull2RefreshTableViewDelegate::onPullXXXComplete(table)`来终止载入的动画，否则那个原本看起来挺可爱旋转小菊花就会令你头痛不已。

##添加到场景中
一般情况下，下拉刷新表会被添加到场景中，当然也不排除是自己创建的较深的节点容器，无论是哪种情况，只需要简单`addChild`即可，否则折腾了半天却看不到效果也许会令你抓狂一阵子。

```
this->addChild(table);
```

#限制
目前仅支持内容垂直模式分布，水平模式不支持，所以`setDirection`函数是否调用关系不大，即使设置了水平模式，也会得到一个输出表示不支持。

#免费使用
呃，你可以在任何场景任何项目任何平台免费自由使用这份代码，可以随意修改，甚至也可以把头上的作者改成你自己，都无所谓。我不是很懂那些GPL或者apache等的版权限制，只要你用得爽，我也会很高兴，当然，有任何Bug或者改进意见都欢迎和我交流。

#联系方式
![作者虚拟头像][4]

作者：*Jason Tou*
邮箱：[*sosoayaen@gmail.com*][3]
twitter：*sosoayaen*
新浪微博：*sosoayaen*

作者公司使用`cocos2dx`作为主要引擎。主要使用`C++`进行开发，也伴有些许的`Lua`脚本。

再一次感谢创造出`CCScrollView`和`CCTableView`的人们，你们太神奇了。有了你们，才令我们的生活更丰富多彩。

  [1]: http://twitter.com "推特"
  [2]: http://t.cn "新浪微博"
  [3]: mailto:sosoayaen@gmail.com "邮箱"
  [4]: https://lh6.googleusercontent.com/-v3RQz2k1kZM/UukfllsCVCI/AAAAAAAAAH8/ty5duHawRg0/s0/348.jpg "Jason Tou"