### Vue结构
```html
        var vm = new Vue({
            el: '#app',         //绑定的区域
            data: {             //控制的区域需要使用的数据对象
                num: 0
            },
            methods: {          //自定义的事件方法对象
                handle1: function (event) {
                    console.log(event.target.innerHTML)
                },
                handle2: function (p, p1, event) {
                    console.log(p, p1)
                    console.log(event.target.innerHTML)
                    this.num++;
                }
            }
        });

<!-- vue提供的标签 -->
        <component :is="componentId"></component>
        <template><div></div></template>
        <transition></transition>
        <transition-group></transition-group>
```
### mvvm

- MVC 是后端的分层开发概念； MVVM是前端视图层的概念，主要关注于 视图层分离，也就是说：MVVM把前端的视图层，分为了 三部分 Model, View , VM ViewModel
- m   model  
  - 数据层   Vue  中 数据层 都放在 data 里面
- v   view     视图   
  - Vue  中  view      即 我们的HTML页面  
- vm   （view-model）     控制器     将数据和视图层建立联系      
  - vm 即  Vue 的实例  就是 vm  

##  指令
### v-cloak      
- 防止页面加载时出现闪烁问题

  ```html
<style type="text/css">
    /* 
      1、通过属性选择器 选择到 带有属性 v-cloak的标签  让他隐藏
   */
    [v-cloak] {
        /* 元素隐藏    */
        display: none;
    }
</style>
<body>
    <div id="app">
        <!-- 2、 让带有插值 语法的   添加 v-cloak 属性 
           在 数据渲染完场之后，v-cloak 属性会被自动去除，
           v-cloak一旦移除也就是没有这个属性了  属性选择器就选择不到该标签
  		 也就是对应的标签会变为可见
      -->
        <div v-cloak>{{msg}}</div>
    </div>
    <script type="text/javascript" src="js/vue.js"></script>
    <script type="text/javascript">
        var vm = new Vue({
            //  el   指定元素 id 是 app 的元素  
            el: '#app',
            //  data  里面存储的是数据
            data: {
                msg: 'Hello Vue'
            }
        });
    </script>
</body>
</html>
  ```

### v-text       
- 用于将数据填充到标签中，作用于插值表达式类似，但是没有闪动问题
- 如果数据中有HTML标签会将html标签一并输出
- 注意：此处为单向绑定，数据对象上的值改变，插值会发生变化；但是当插值发生变化并不会影响数据对象的值

```html
<div id="app">
    <!--  
		注意:在指令中不要写插值语法  直接写对应的变量名称 
        在 v-text 中 赋值的时候不要在写 插值语法
		一般属性中不加 {{}}  直接写 对应 的数据名 
	-->
    <p v-text="msg"></p>
    <p>
        <!-- Vue  中只有在标签的 内容中 才用插值语法 -->
        {{msg}}
    </p>
</div>
<script>
    new Vue({
        el: '#app',
        data: {
            msg: 'Hello Vue.js'
        }
    });

</script>
```

### v-html
- 用法和v-text相似  但是他可以将HTML片段填充到标签中
- 可能有安全问题, 一般只在可信任内容上使用 `v-html`，**永不**用在用户提交的内容上
- 它与v-text区别在于v-text输出的是纯文本，浏览器不会对其再进行html解析，但v-html会将其当html标签解析后输出。
  ```html
<div id="app">
    　　<p v-html="html"></p> <!-- 输出：html标签在渲染的时候被解析 -->
    <p>{{message}}</p> <!-- 输出：<span>通过双括号绑定</span> -->
    　　<p v-text="text"></p> <!-- 输出：<span>html标签在渲染的时候被源码输出</span> -->
</div>
<script>
    let app = new Vue({
        el: "#app",
        data: {
            message: "<span>通过双括号绑定</span>",
            html: "<span>html标签在渲染的时候被解析</span>",
            text: "<span>html标签在渲染的时候被源码输出</span>",
        }
    });
</script>
  ```

  ### v-bind

- v-bind 指令被用来响应地更新 HTML 属性
- v-bind:href    可以缩写为    :href;

```html
<!-- 绑定一个属性 -->
<img v-bind:src="imageSrc">
<!-- 缩写 -->
<img :src="imageSrc">
```
#### 绑定样式的两种方法
1. 绑定class
- 我们可以给v-bind:class 一个对象，以动态地切换class。
- 注意：v-bind:class指令可以与普通的class特性共存
- 如果绑定的是一个对象 则 键为 对应的类名 值 为对应data中的数据
- 如果绑定的是一个数组  数组中classA和 classB 对应为data中的数据
```html
    <!-- 1.绑定的是一个对象 -->
<!-- 
	HTML最终渲染为 <ul class="box textColor textSize"></ul>
	注意：
		textColor，textSize  对应的渲染到页面上的CSS类名	
		isColor，isSize  对应vue data中的数据  如果为true 则对应的类名 渲染到页面上 
		当 isColor 和 isSize 变化时，class列表将相应的更新，
		例如，将isSize改成false，
		class列表将变为 <ul class="box textColor"></ul>
-->
<ul class="box" v-bind:class="{textColor:isColor, textSize:isSize}">
    <li>学习Vue</li>
    <li>学习Node</li>
    <li>学习React</li>
</ul>
<div v-bind:style="{color:activeColor,fontSize:activeSize}">对象语法</div>
<script>
    var vm= new Vue({
        el:'.box',
        data:{
            isColor:true,
            isSize:true，
            activeColor:"red",
            activeSize:"25px",
        }
    })
</script>
<style>
    .box {
        border: 1px dashed #f0f;
    }
    .textColor {
        color: #f00;
        background-color: #eef;
    }
    .textSize {
        font-size: 30px;
        font-weight: bold;
    }
</style>
```

```html
<ul class="box" :class="[classA, classB]">
    <li>学习Vue</li>
    <li>学习Node</li>
    <li>学习React</li>
</ul>
<script>
    var vm = new Vue({
        el: '.box',
        data: {
            classA: 'textColor',
            classB: 'textSize'
        }
    })
</script>
<style>
    .box {
        border: 1px dashed #f0f;
    }
    .textColor {
        color: #f00;
        background-color: #eef;
    }
    .textSize {
        font-size: 30px;
        font-weight: bold;
    }
</style>
```

2. 绑定style

```html
<div v-bind:style="styleObject">绑定样式对象</div>'
<!-- CSS 属性名可以用驼峰式 (camelCase) 或短横线分隔 (kebab-case，记得用单引号括起来)    -->
<div v-bind:style="{ color: activeColor, fontSize: fontSize,background:'red' }">内联样式</div>
<!--组语法可以将多个样式对象应用到同一个元素 -->
<div v-bind:style="[styleObj1, styleObj2]"></div>
<script>
    new Vue({
        el: '#app',
        data: {
            styleObject: {
                color: 'green',
                fontSize: '30px',
                background: 'red'
            },
            activeColor: 'green',
            fontSize: "30px"
        },
        styleObj1: {
            color: 'red'
        },
        styleObj2: {
            fontSize: '30px'
        }
</script>
```


### v-pre
- 显示原始信息跳过编译过程
- 跳过这个元素和它的子元素的编译过程。
- **一些静态的内容不需要编译加这个指令可以加快渲染**

```html
<span v-pre>{{ this will not be compiled }}</span>
<!--  显示的是{{ this will not be compiled }}  -->
<span v-pre>{{msg}}</span> 
<!--   即使data里面定义了msg这里仍然是显示的{{msg}}  -->
<script>
    new Vue({
        el: '#app',
        data: {
            msg: 'Hello Vue.js'
        }
    });
</script>
```

### v-once

- 执行一次性的插值【当数据改变时，插值处的内容不会继续更新】

```html
<!-- 即使data里面定义了msg 后期我们修改了 仍然显示的是第一次data里面存储的数据即 Hello Vue.js  -->
<span v-once>{{ msg}}</span>
<script>
    new Vue({
        el: '#app',
        data: {
            msg: 'Hello Vue.js'
        }
    });
</script>
```

### v-model
- 双向数据绑定
- 当数据发生变化的时候，视图也就发生变化
- 当视图发生变化的时候，数据也会跟着同步变化
- **v-model**是一个指令，限制在 `<input>、<select>、<textarea>、components`中使用
```html
<div id="app">
    <div>{{msg}}</div>
    <div>
        <!-- 当输入框中内容改变的时候， 页面上的msg 会自动更新 -->
        <input type="text" v-model='msg'>
    </div>
</div>
```


###   v-on

- 用来绑定事件的
-  形式如：v-on:click  缩写为 @click;
```html
<body>
    <div id="app">
        <div>{{num}}</div>
        <div>
            <!-- 如果事件直接绑定函数名称，那么默认会传递事件对象作为事件函数的第一个参数 -->
            <button v-on:click='handle1'>点击1</button>
            <!-- 2、如果事件绑定函数调用，那么事件对象必须作为最后一个参数显示传递，
                 并且事件对象的名称必须是$event 
            -->
            <button @:click='handle2(123, 456, $event)'>点击2</button>
        </div>
    </div>
    <script type="text/javascript" src="js/vue.js"></script>
    <script type="text/javascript">
        var vm = new Vue({
            el: '#app',
            data: {
                num: 0
            },
            methods: {
                handle1: function (event) {
                    console.log(event.target.innerHTML)
                },
                handle2: function (p, p1, event) {
                    console.log(p, p1)
                    console.log(event.target.innerHTML)
                    this.num++;
                }
            }
        });
    </script>
</body>
```

### v-if
- 1- 多个元素 通过条件判断展示或者隐藏某个元素。或者多个元素
- 2- 进行两个视图之间的切换
- v-if是动态的向DOM树内添加或者删除DOM元素
  - v-if切换有一个局部编译/卸载的过程，切换过程中合适地销毁和重建内部的事件监听和子组件

```html
<div id="app">
    <!--  判断是否加载，如果为真，就加载，否则不加载-->
    <span v-if="flag">
        如果flag为true则显示,false不显示!
    </span>
</div>
<script>
    var vm = new Vue({
        el: "#app",
        data: {
            flag: true
        }
    })
</script>
----------------------------------------------------------
<div v-if="type === 'A'">
    A
</div>
<!-- v-else-if紧跟在v-if或v-else-if之后   表示v-if条件不成立时执行-->
<div v-else-if="type === 'B'">
    B
</div>
<div v-else-if="type === 'C'">
    C
</div>
<!-- v-else紧跟在v-if或v-else-if之后-->
<div v-else>
    Not A/B/C
</div>
<script>
    new Vue({
        el: '#app',
        data: {
            type: 'C'
        }
    })
</script>
```

### v-show

- v-show本质就是标签display设置为none，控制隐藏
  - v-show只编译一次，后面其实就是控制css，而v-if不停的销毁和创建，故v-show性能更好一点。

### v-for
- 用于循环的数组里面的值可以是对象，也可以是普通元素  

```html
<ul id="example-1">
    <!-- 循环结构-遍历数组  
     item 是我们自己定义的一个名字  代表数组里面的每一项  
     items对应的是 data中的数组-->
    <li v-for="item in items">
        {{ item.message }}
    </li>
</ul>
<script>
    new Vue({
        el: '#example-1',
        data: {
            items: [
                { message: 'Foo' },
                { message: 'Bar' }
            ]，

        }
    })
</script>
```
- **不推荐**同时使用 `v-if` 和 `v-for`
- 当 `v-if` 与 `v-for` 一起使用时，`v-for` 具有比 `v-if` 更高的优先级。

```html
<!--  循环结构-遍历对象
		v 代表   对象的value
		k  代表对象的 键 
		i  代表索引	
	--->
<div v-if='v==13' v-for='(v,k,i) in obj'>{{v + '---' + k + '---' + i}}</div>
<script>
    new Vue({
        el: '#example-1',
        data: {
            items: [
                { message: 'Foo' },
                { message: 'Bar' }
            ]，
            obj: {
                uname: 'zhangsan',
                age: 13,
                gender: 'female'
            }
        }
    })
</script>
```
- key 的作用
  - **key来给每个节点做一个唯一标识**
  - **key的作用主要是为了高效的更新虚拟DOM**
  - 只接受string / number参数
```html
<ul>
  <li v-for="item in items" :key="item.id">...</li>
</ul>

```

###  事件修饰符

- 在事件处理程序中调用 `event.preventDefault()` 或 `event.stopPropagation()` 是非常常见的需求。
- Vue 不推荐我们操作DOM    为了解决这个问题，Vue.js 为 `v-on` 提供了**事件修饰符**
- 修饰符是由点开头的指令后缀来表示的

```html
<!-- 阻止单击事件继续传播冒泡 -->
<a v-on:click.stop="doThis"></a>

<!-- prevent阻止默认行为 -->
<form v-on:submit.prevent="onSubmit"></form>

<!-- 修饰符可以串联   即阻止冒泡也阻止默认事件 -->
<a v-on:click.stop.prevent="doThat"></a>

<!-- 捕获机制 -->
<a v-on:click.capture="doThat"></a>

<!-- 只当在 event.target 是当前元素自身时触发处理函数 -->
<!-- 即事件不是从内部元素触发的 -->
<div v-on:click.self="doThat">...</div>

<!-- 只执行一次 -->
        <a href="http://www.baidu.com" @click.prevent="linkClick">有问题请百度</a> <br>
        <a href="http://www.baidu.com" @click.prevent.once="linkClick">有问题请百度1</a>
<a v-on:click.once="doThat"></a>

使用修饰符时，顺序很重要；相应的代码会以同样的顺序产生。因此，用 v-on:click.prevent.self 会阻止所有的点击，而 v-on:click.self.prevent 只会阻止对元素自身的点击。
```


### 过滤器

- Vue.js允许自定义过滤器，可被用于一些常见的文本格式化。
- 过滤器可以用在两个地方：双花括号插值和v-bind表达式。
- 过滤器应该被添加在JavaScript表达式的尾部，由“管道”符号指示
- 支持级联操作
- 过滤器不改变真正的`data`，而只是改变渲染的结果，并返回过滤后的版本
- 全局注册时是filter，没有s的。而局部过滤器是filters，是有s的


```html
<div id="app">
    <input type="text" v-model='msg'>
    <!-- upper 被定义为接收单个参数的过滤器函数，表达式  msg  的值将作为参数传入到函数中 -->
    <div>{{msg | upper}}</div>
    <!--  
      支持级联操作
      upper  被定义为接收单个参数的过滤器函数，表达式msg 的值将作为参数传入到函数中。
	  然后继续调用同样被定义为接收单个参数的过滤器 lower ，将upper 的结果传递到lower中
 	-->
    <div>{{msg | upper | lower}}</div>
    <div :abc='msg | upper'>测试数据</div>
</div>

<script type="text/javascript">
    //  lower  为全局过滤器     
    Vue.filter('lower', function (val) {
        return val.charAt(0).toLowerCase() + val.slice(1);
    });
    var vm = new Vue({
        el: '#app',
        data: {
            msg: ''
        },
        //filters  属性 定义 和 data 已经 methods 平级 
        //  定义filters 中的过滤器为局部过滤器 
        filters: {
            //   upper  自定义的过滤器名字 
            //    upper 被定义为接收单个参数的过滤器函数，表达式  msg  的值将作为参数传入到函数中
            upper: function (val) {
                //  过滤器中一定要有返回值 这样外界使用过滤器的时候才能拿到结果
                return val.charAt(0).toUpperCase() + val.slice(1);
            }
        }
    });
</script>
```

```html
<!-- 过滤器中传递参数 -->
<div id="box">
    <!--
        filterA 被定义为接收三个参数的过滤器函数。
          其中 message 的值作为第一个参数，
        普通字符串 'arg1' 作为第二个参数，表达式 arg2 的值作为第三个参数。
    -->
    {{ message | filterA('arg1', 'arg2') }}
</div>
<script>
    // 在过滤器中 第一个参数 对应的是  管道符前面的数据   n  此时对应 message
    // 第2个参数  a 对应 实参  arg1 字符串
    // 第3个参数  b 对应 实参  arg2 字符串
    Vue.filter('filterA', function (n, a, b) {
        if (n < 10) {
            return n + a;
        } else {
            return n + b;
        }
    });
    new Vue({
        el: "#box",
        data: {
            message: "哈哈哈"
        }
    })
</script>
```


### 按键修饰符

- 在做项目中有时会用到键盘事件，在监听键盘事件时，我们经常需要检查详细的按键。Vue 允许为 `v-on` 在监听键盘事件时添加按键修饰符

```html
<!-- 只有在 `keyCode` 是 13 时调用 `vm.submit()` -->
<input v-on:keyup.13="submit">

<!-- -当点击enter 时调用 `vm.submit()` -->
<input v-on:keyup.enter="submit">

<!--当点击enter或者space时  时调用 `vm.alertMe()`   -->
<input type="text" v-on:keyup.enter.space="alertMe" >

常用的按键修饰符
.enter =>    enter键
.tab => tab键
.delete (捕获“删除”和“退格”按键) =>  删除键
.esc => 取消键
.space =>  空格键
.up =>  上
.down =>  下
.left =>  左
.right =>  右


<script>
	var vm = new Vue({
        el:"#app",
        methods: {
              submit:function(){},
              alertMe:function(){},
        }
    })

</script>
```

- 自定义按键修饰符别名
- 在Vue中可以通过`config.keyCodes`自定义按键修饰符别名

```html
<div id="app">
    预先定义了keycode 116（即F5）的别名为f5，因此在文字输入框中按下F5，会触发prompt方法
    <input type="text" v-on:keydown.f5="prompt()">
</div>

<script>
	
    Vue.config.keyCodes.f5 = 116;

    let app = new Vue({
        el: '#app',
        methods: {
            prompt: function() {
                alert('我是 F5！');
            }
        }
    });
</script>
```

###  自定义指令

- 内置指令不能满足我们特殊的需求
- Vue允许我们自定义指令
- 指令定义函数提供了几个钩子函数（可选）：
- bind：只调用一次，指令第一次绑定到元素时调用，用这个钩子函数可以定义一个在绑定时执行一次的初始化动作。
- inserted：被绑定元素插入父节点时调用（父节点存在即可调用，不必存在于 document 中）。
- update：第一次是紧跟在 bind 之后调用，获得的参数是绑定的初始值，之后被绑定元素所在的模板更新时调用，而不论绑定值是否变化。通过比较更新前后的绑定值，可以忽略不必要的模板更新（详细的钩子函数参数见下）。
- componentUpdated：被绑定元素所在模板完成一次更新周期时调用。
- unbind：只调用一次， 指令与元素解绑时调用。

#### Vue.directive  注册全局指令

```html
<!-- 
  使用自定义的指令，只需在对用的元素中，加上'v-'的前缀形成类似于内部指令'v-if'，'v-text'的形式。 
-->
<input type="text" v-focus>
<script>
// 注意点： 
//   1、 在自定义指令中  如果以驼峰命名的方式定义 如  Vue.directive('focusA',function(){}) 
//   2、 在HTML中使用的时候 只能通过 v-focus-a 来使用 
    
// 注册一个全局自定义指令 v-focus
Vue.directive('focus', {
  	// 当绑定元素插入到 DOM 中。 其中 el为dom元素
  	inserted: function (el) {
    		// 聚焦元素
    		el.focus();
 	}
});
new Vue({
　　el:'#app'
});
</script>
```


#### Vue.directive  注册全局指令 带参数

```html
  <input type="text" v-color='msg'>
 <script type="text/javascript">
    /*
      自定义指令-带参数
      bind - 只调用一次，在指令第一次绑定到元素上时候调用

    */
    Vue.directive('color', {
      // bind声明周期, 只调用一次，指令第一次绑定到元素时调用。在这里可以进行一次性的初始化设置
      // el 为当前自定义指令的DOM元素  
      // binding 为自定义的函数形参   通过自定义属性传递过来的值 存在 binding.value 里面
      bind: function(el, binding){
        // 根据指令的参数设置背景色
        // console.log(binding.value.color)
        el.style.backgroundColor = binding.value.color;
      }
    });
    var vm = new Vue({
      el: '#app',
      data: {
        msg: {
          color: 'blue'
        }
      }
    });
  </script>
```

#### 自定义指令局部指令

- 局部指令，需要定义在  directives 的选项   用法和全局用法一样 
- 局部指令只能在当前组件里面使用
- 当全局指令和局部指令同名时以局部指令为准

```html
<input type="text" v-color='msg'>
 <input type="text" v-focus>
 <script type="text/javascript">
    /*
      自定义指令-局部指令
    */
    var vm = new Vue({
      el: '#app',
      data: {
        msg: {
          color: 'red'
        }
      },
   	  //局部指令，需要定义在  directives 的选项
      directives: {
        color: {
          bind: function(el, binding){
            el.style.backgroundColor = binding.value.color;
          }
        },
        focus: {
          inserted: function(el) {
            el.focus();
          }
        }
        color:function(el, binding){
            el.style.backgroundColor = binding.value.color;
        },
      }
    });
  </script>
```


### 生命周期

- 事物从出生到死亡的过程
- Vue实例从创建 到销毁的过程 ，这些过程中会伴随着一些函数的自调用。我们称这些函数为钩子函数

####常用的 钩子函数

| beforeCreate  | 在实例初始化之后，数据观测和事件配置之前被调用 此时data 和 methods 以及页面的DOM结构都没有初始化   什么都做不了 |
| ------------- | ------------------------------------------------------------ |
| created       | 在实例创建完成后被立即调用此时data 和 methods已经可以使用  但是页面还没有渲染出来 |
| beforeMount   | 在挂载开始之前被调用   此时页面上还看不到真实数据 只是一个模板页面而已 |
| mounted       | el被新创建的vm.$el替换，并挂载到实例上去之后调用该钩子。  数据已经真实渲染到页面上  在这个钩子函数里面我们可以使用一些第三方的插件 |
| beforeUpdate  | 数据更新时调用，发生在虚拟DOM打补丁之前。   页面上数据还是旧的 |
| updated       | 由于数据更改导致的虚拟DOM重新渲染和打补丁，在这之后会调用该钩子。 页面上数据已经替换成最新的 |
| beforeDestroy | 实例销毁之前调用                                             |
| destroyed     | 实例销毁后调用     





### 动画
 v-enter -------------> v-enter-to
         v-enter-active

 v-leave -------------> v-leave-to
         v-leave-active

- 钩子函数
```html
<transition
  v-on:before-enter="beforeEnter"
  v-on:enter="enter"
  v-on:after-enter="afterEnter"
  v-on:enter-cancelled="enterCancelled"

  v-on:before-leave="beforeLeave"
  v-on:leave="leave"
  v-on:after-leave="afterLeave"
  v-on:leave-cancelled="leaveCancelled"
>
  <!-- ... -->
</transition>
```



