//index.js
//获取应用实例
const app = getApp()
Page({
  data: {
    current: 'homepage',
   },

  countDown: function () {
    let that = this;
    let countDownNum = that.data.countDownNum;//获取倒计时初始值

    //如果将定时器设置在外面，那么用户就看不到countDownNum的数值动态变化，所以要把定时器存进data里面
    that.setData({
      timer: setInterval(function () {//这里把setInterval赋值给变量名为timer的变量


      var  test2 = app.globalData.test2
        that.setData({
          test2: test2
        })



        const requestTask = wx.request({
          url: 'https://api.heclouds.com/devices/503071333/datapoints?datastream_id=Light,Temperature&limit=15',
          header: {
            'content-type': 'application/json',
            'api-key': 'g81xoJ5mSA4t=vQLvEDIaUKkZ60='
          },
          success: function (res) {
            //console.log(res.data)
            //拿到数据后保存到全局数据
            var app = getApp()
            app.globalData.temperature = res.data.data.datastreams[0]
            app.globalData.light = res.data.data.datastreams[1]
            console.log(app.globalData.light)
            if (app.globalData.temperature.datapoints[0].value>app.globalData.test)
            {
              app.globalData.test2 = app.globalData.temperature.datapoints[0].value
              test2 = app.globalData.test2
              that.setData({
                test2: test2
              })
            }
            var test = app.globalData.temperature.datapoints[0].value
            that.setData({
              test: test
            })
           
          },
          fail: function (res) {
            console.log("fail!!!")
          },

          complete: function (res) {
            console.log("end")
          }
        })




        countDownNum--;
        that.setData({
          countDownNum: countDownNum
        })

      }, 1000)
    })
  },


  handleChange({ detail }) {
    wx.redirectTo({
      url: '../a/a',
    })
    this.setData({
      current: detail.key
    });
  },/*导航栏组件*/

  
  onLoad: function () {
  },
  


  onShow: function () {
    this.countDown();
  },
})
