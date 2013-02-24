# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyKDE4.kdecore import *
from PyKDE4.kdeui import *
from PyKDE4.kio import *
from PyKDE4.plasma import Plasma
from PyKDE4 import plasmascript
from PyQt4 import QtCore
from configwindow import *
from config import *
from util import *
import commands, os, time



class pyTextWidget(plasmascript.Applet):
    def __init__(self,parent, args=None):
        """widget definition"""
        plasmascript.Applet.__init__(self,parent)

    def init(self):
        """function to initializate widget"""
        self._name = str(self.package().metadata().pluginName())
        self.layout = QGraphicsLinearLayout(Qt.Horizontal, self.applet)
        self.setHasConfigurationInterface(True)
        self.resize(10,10)
        
        # initial configuration
        self.settings = Config(self)
        self.interval = int(self.settings.get('interval', '2000'))
        self.font_family = str(self.settings.get('font_family', 'Terminus'))
        self.font_size = int(self.settings.get('font_size', 12))
        self.font_color = str(self.settings.get('font_color', '#000000'))
        self.font_style = str(self.settings.get('font_style', 'normal'))
        self.formatLine = "<html><head/><body style=\" font-family:\'" + self.font_family + "\'; font-size:" + str(self.font_size)
        self.formatLine = self.formatLine +"pt; font-style:" + self.font_style +";\">"
        self.formatLine = self.formatLine + "<p align=\"center\"><span style=\" color:" + self.font_color + ";\"><pre>$LINE"
        self.formatLine = self.formatLine + "</pre></span></p></body></html>"
        
        self.cpuBool = self.settings.get('cpuBool', True)
        if (self.cpuBool):
            self.cpuFormat = str(self.settings.get('cpuFormat', '[cpu: $cpu%]'))
            self.label_cpu = Plasma.Label(self.applet)
            text = self.formatLine.split('$LINE')[0] + self.cpuFormat.split('$cpu')[0] + '-----' + self.cpuFormat.split('$cpu')[1] + self.formatLine.split('$LINE')[1]
            self.label_cpu.setText(text)
            self.layout.addItem(self.label_cpu)
        
        self.tempBool = self.settings.get('tempBool', True)
        if (self.tempBool):
            self.tempFormat = str(self.settings.get('tempFormat', '[temp: $temp&deg;C]'))
            self.setupTemp()
            self.label_temp = Plasma.Label(self.applet)
            text = self.formatLine.split('$LINE')[0] + self.tempFormat.split('$temp')[0] + '----' + self.tempFormat.split('$temp')[1] + self.formatLine.split('$LINE')[1]
            self.label_temp.setText(text)
            self.layout.addItem(self.label_temp)
        
        self.memBool = self.settings.get('memBool', True)
        if (self.memBool):
            self.memFormat = str(self.settings.get('memFormat', '[mem: $mem%]'))
            if (self.memFormat.split('$memmb')[0] != self.memFormat):
                self.memInMb = True
                text = self.formatLine.split('$LINE')[0] + self.memFormat.split('$memmb')[0] + '-----' + self.memFormat.split('$memmb')[1] + self.formatLine.split('$LINE')[1]
            else:
                self.memInMb = False
                self.mem_used = 0.0
                self.mem_free = 1.0
                self.mem_uf = 0.0
                text = self.formatLine.split('$LINE')[0] + self.memFormat.split('$mem')[0] + '-----' + self.memFormat.split('$mem')[1] + self.formatLine.split('$LINE')[1]
            self.label_mem = Plasma.Label(self.applet)
            self.label_mem.setText(text)
            self.layout.addItem(self.label_mem)
        
        self.swapBool = self.settings.get('swapBool', True)
        if (self.swapBool):
            self.swapFormat = str(self.settings.get('swapFormat', '[swap: $swap%]'))
            if (self.swapFormat.split('$swapmb')[0] != self.swapFormat):
                self.swapInMb = True
                text = self.formatLine.split('$LINE')[0] + self.swapFormat.split('$swapmb')[0] + '-----' + self.swapFormat.split('$swapmb')[1] + self.formatLine.split('$LINE')[1]
            else:
                self.swapInMb = False
                text = self.formatLine.split('$LINE')[0] + self.swapFormat.split('$swap')[0] + '-----' + self.swapFormat.split('$swap')[1] + self.formatLine.split('$LINE')[1]
                self.swap_free = 1.0
                self.swap_used = 0.0
            self.label_swap = Plasma.Label(self.applet)
            self.label_swap.setText(text)
            self.layout.addItem(self.label_swap)
            
        self.netBool = self.settings.get('netBool', True)
        if (self.netBool):
            self.netNonFormat = str(self.settings.get('netNonFormat', '[net: $netKB/s]'))
            if (self.netNonFormat.split('@@netdev=')[0] != self.netNonFormat):
                self.netdev = self.netNonFormat.split('@@')[1].split('netdev=')[1]
                self.netNonFormat = self.netNonFormat.split('@@')[0] + self.netNonFormat.split('@@')[2]
            else:
                self.num_dev = int(self.settings.get('num_dev', 0))
                self.setupNetdev()
            if (self.netNonFormat.split('$netdev')[0] != self.netNonFormat):
                self.netFormat = self.netNonFormat.split('$netdev')[0] + self.netdev + self.netNonFormat.split('$netdev')[1]
            else:
                self.netFormat = self.netNonFormat
            self.label_netDown = Plasma.Label(self.applet)
            text = self.formatLine.split('$LINE')[0] + self.netFormat.split('$net')[0] + '----' + self.formatLine.split('$LINE')[1]
            self.label_netDown.setText(text)
            self.layout.addItem(self.label_netDown)
            self.label_netUp = Plasma.Label(self.applet)
            text = self.formatLine.split('$LINE')[0] + '/----' + self.netFormat.split('$net')[1] + self.formatLine.split('$LINE')[1]
            self.label_netUp.setText(text)
            self.layout.addItem(self.label_netUp)
            
        self.batBool = self.settings.get('batBool', True)
        if (self.batBool):
            self.batFormat = str(self.settings.get('batFormat', '[bat: $bat%]'))
            self.label_bat = Plasma.Label(self.applet)
            text = self.formatLine.split('$LINE')[0] + self.batFormat.split('$bat')[0] + '---' + self.batFormat.split('$bat')[1] + self.formatLine.split('$LINE')[1]
            self.label_bat.setText(text)
            self.layout.addItem(self.label_bat)
        
        self.applet.setLayout(self.layout)        
        self.theme = Plasma.Svg(self)
        self.theme.setImagePath("widgets/background")
        self.setBackgroundHints(Plasma.Applet.DefaultBackground)
        
        # start timer
        self.connectToEngine()
        self.timer = QtCore.QTimer()
        self.timer.setInterval(self.interval)
        self.startPolling()
     
    def configAccepted(self):
        """function to accept settings"""
        # update local variables
        self.interval = int(self.configpage.ui.spinBox_interval.value())
        self.settings.set('interval', self.interval)
        self.font_family = str(self.configpage.ui.fontComboBox.currentFont().family())
        self.settings.set('font_family', self.font_family)
        self.font_size = int(self.configpage.ui.spinBox_fontSize.value())
        self.settings.set('font_size', self.font_size)
        self.font_color = str(self.configpage.ui.kcolorcombo.color().name())
        self.settings.set('font_color', self.font_color)
        self.font_style = str(self.configpage.ui.lineEdit_style.text())
        self.settings.set('font_style', self.font_style)
        self.formatLine = "<html><head/><body style=\" font-family:\'" + self.font_family + "\'; font-size:" + str(self.font_size)
        self.formatLine = self.formatLine +"pt; font-style:" + self.font_style +";\">"
        self.formatLine = self.formatLine + "<p align=\"center\"><span style=\" color:" + self.font_color + ";\"><pre>$LINE"
        self.formatLine = self.formatLine + "</pre></span></p></body></html>"
        
        if (self.configpage.ui.checkBox_cpu.checkState() == 2):
            self.cpuBool = True
            self.cpuFormat = str(self.configpage.ui.lineEdit_cpu.text())
            self.settings.set('cpuFormat', self.cpuFormat)
        else:
            self.cpuBool = False
        self.settings.set('cpuBool', self.cpuBool)
        
        if (self.configpage.ui.checkBox_temp.checkState() == 2):
            self.tempBool = True
            self.tempFormat = str(self.configpage.ui.lineEdit_temp.text())
            self.settings.set('tempFormat', self.tempFormat)
        else:
            self.tempBool = False
        self.settings.set('tempBool', self.tempBool)
        
        if (self.configpage.ui.checkBox_mem.checkState() == 2):
            self.memBool = True
            self.memFormat = str(self.configpage.ui.lineEdit_mem.text())
            self.settings.set('memFormat', self.memFormat)
        else:
            self.memBool = False
        self.settings.set('memBool', self.memBool)
        
        if (self.configpage.ui.checkBox_swap.checkState() == 2):
            self.swapBool = True
            self.swapFormat = str(self.configpage.ui.lineEdit_swap.text())
            self.settings.set('swapFormat', self.swapFormat)
        else:
            self.swapBool = False
        self.settings.set('swapBool', self.swapBool)
        
        if (self.configpage.ui.checkBox_net.checkState() == 2):
            self.netBool = True
            self.netFormat = str(self.configpage.ui.lineEdit_net.text())
            self.num_dev = int(self.configpage.ui.comboBox_numNet.currentIndex())
            self.settings.set('netFormat', self.netFormat)
            self.settings.set('num_dev', self.num_dev)
        else:
            self.netBool = False
        self.settings.set('netBool', self.netBool)
        
        if (self.configpage.ui.checkBox_bat.checkState() == 2):
            self.batBool = True
            self.batFormat = str(self.configpage.ui.lineEdit_bat.text())
            self.settings.set('batFormat', self.batFormat)
        else:
            self.batBool = False
        self.settings.set('batBool', self.batBool)        
        
        # update timer
        self.timer.setInterval(self.interval)
        self.startPolling()
    
    def createConfigurationInterface(self, parent):
        """function to setup configuration window"""
        self.configpage = ConfigWindow(self, self.settings)
        
        font = QFont(str(self.settings.get('font_family', 'Terminus')), int(self.settings.get('font_size', 12)), int(int(self.settings.get('font_weight', 50))))
        self.configpage.ui.spinBox_interval.setValue(int(self.settings.get('interval', '2000')))
        self.configpage.ui.fontComboBox.setCurrentFont(font)
        self.configpage.ui.spinBox_fontSize.setValue(int(self.settings.get('font_size', 12)))
        self.configpage.ui.kcolorcombo.setColor(QColor(str(self.settings.get('font_color', '#000000'))))
        self.configpage.ui.lineEdit_style.setText(str(self.settings.get('font_style', 'normal')))
        
        if (self.cpuBool):
            self.configpage.ui.checkBox_cpu.setCheckState(2)
            self.configpage.ui.lineEdit_cpu.setEnabled(True)
            self.configpage.ui.lineEdit_cpu.setText(str(self.settings.get('cpuFormat', '[cpu: $cpu%]')))
        else:
            self.configpage.ui.checkBox_cpu.setCheckState(0)
            self.configpage.ui.lineEdit_cpu.setDisabled(True)
        
        if (self.tempBool):
            self.configpage.ui.checkBox_temp.setCheckState(2)
            self.configpage.ui.lineEdit_temp.setEnabled(True)
            self.configpage.ui.lineEdit_temp.setText(str(self.settings.get('tempFormat', '[temp: $temp&deg;C]')))
        else:
            self.configpage.ui.checkBox_temp.setCheckState(0)
            self.configpage.ui.lineEdit_temp.setDisabled(True)
        
        if (self.memBool):
            self.configpage.ui.checkBox_mem.setCheckState(2)
            self.configpage.ui.lineEdit_mem.setEnabled(True)
            self.configpage.ui.lineEdit_mem.setText(str(self.settings.get('memFormat', '[mem: $mem%]')))
        else:
            self.configpage.ui.checkBox_mem.setCheckState(0)
            self.configpage.ui.lineEdit_mem.setDisabled(True)
        
        if (self.swapBool):
            self.configpage.ui.checkBox_swap.setCheckState(2)
            self.configpage.ui.lineEdit_swap.setEnabled(True)
            self.configpage.ui.lineEdit_swap.setText(str(self.settings.get('swapFormat', '[swap: $swap%]')))
        else:
            self.configpage.ui.checkBox_swap.setCheckState(0)
            self.configpage.ui.lineEdit_swap.setDisabled(True)
        
        if (self.netBool):
            self.configpage.ui.checkBox_net.setCheckState(2)
            self.configpage.ui.lineEdit_net.setEnabled(True)
            self.configpage.ui.comboBox_numNet.setEnabled(True)
            self.configpage.ui.comboBox_numNet.setCurrentIndex(int(self.settings.get('num_dev', 0)))
            self.configpage.ui.lineEdit_net.setText(str(self.settings.get('netNonFormat', '[net: $net%]')))
        else:
            self.configpage.ui.checkBox_swap.setCheckState(0)
            self.configpage.ui.comboBox_numNet.setDisabled(True)
            self.configpage.ui.lineEdit_swap.setDisabled(True)
        
        if (self.batBool):
            self.configpage.ui.checkBox_bat.setCheckState(2)
            self.configpage.ui.lineEdit_bat.setEnabled(True)
            self.configpage.ui.lineEdit_bat.setText(str(self.settings.get('batFormat', '[bat: $bat%]')))
        else:
            self.configpage.ui.checkBox_bat.setCheckState(0)
            self.configpage.ui.lineEdit_bat.setDisabled(True)
        
        # add config page
        page = parent.addPage(self.configpage, i18n(self.name()))
        page.setIcon(KIcon(self.icon()))
        
        parent.okClicked.connect(self.configAccepted)
    
    def setupNetdev(self):
        """function to setup network device"""
        if (self.num_dev == 0):
            for line in commands.getoutput("ifconfig -a").split("\n"):
                if (line != ''):
                    if ((line[0] != ' ') and (line[0:3] != "lo:")):
                        self.netdev = line.split()[0][:-1]
        else:
            interfaces = []
            for line in commands.getoutput("ifconfig -a").split("\n"):
                if (line != ''):
                    if ((line[0] != ' ') and (line[0:3] != "lo:")):
                        interfaces.append(line.split()[0][:-1])
                    
            command_line = "if ! (ifconfig "+ interfaces[1] + " | grep 'inet ' > /dev/null); then "
            command_line = command_line + "if ! (ifconfig "+ interfaces[0] + " | grep 'inet ' > /dev/null); then echo lo; "
            command_line = command_line + "else echo "+ interfaces[0] + "; fi; else echo "+ interfaces[1] + "; fi"
            self.netdev = commands.getoutput(command_line)
        
    def setupTemp(self):
        """function to setup temp device"""
        commandOut = commands.getoutput("sensors | grep Physical -B2")
        self.tempdev = "lmsensors/"+commandOut.split("\n")[0]+"/"+'_'.join(commandOut.split("\n")[2].split(":")[0].split())
    
    def showConfigurationInterface(self):
        """function to show configuration window"""
        plasmascript.Applet.showConfigurationInterface(self)
    
    def showTooltip(self, text):
        """function to create and set tooltip"""
        tooltip = Plasma.ToolTipContent()
        tooltip.setImage(KIcon(self.icon()))
        tooltip.setSubText(text)
        tooltip.setAutohide(False)
        Plasma.ToolTipManager.self().setContent(self.applet, tooltip)
        Plasma.ToolTipManager.self().registerWidget(self.applet)
      
    def startPolling(self):
        try:
            self.timer.start()
            QtCore.QObject.connect(self.timer, QtCore.SIGNAL("timeout()"), self.updateLabel)
            
            self.updateLabel()
            self.showTooltip('')
        except Exception as (strerror):
            self.showTooltip(str(strerror))
            self.label.setText('<font color="red">ERROR</font>')
            return
   
    def updateLabel(self):
        """function to update label"""
        if ((self.memBool) and (self.memInMb == False)):
            self.memText()
        if ((self.swapBool) and (self.swapInMb == False)):
            self.swapText()
        if (self.batBool):
            self.batText()
    
    def batText(self):
        """function to set battery text"""
        commandOut = commands.getoutput("acpi")
        bat = "%3s" % (commandOut.split()[3].split("%")[0])
        line = self.batFormat.split('$bat')[0] + bat + self.batFormat.split('$bat')[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_bat.setText(text)
    
    def memText(self):
        """function to set mem text"""
        full = self.mem_uf + self.mem_free
        mem = 100 * self.mem_used / full
        mem = "%5s" % (str(round(mem, 1)))
        line = self.memFormat.split('$mem')[0] + mem + self.memFormat.split('$mem')[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_mem.setText(text)
    
    def swapText(self):
        """function to set swap text"""
        full = self.swap_used + self.swap_free
        mem = 100 * self.swap_used / full
        mem = "%5s" % (str(round(mem, 1)))
        line = self.swapFormat.split('$swap')[0] + mem + self.swapFormat.split('$swap')[1]
        text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
        self.label_swap.setText(text)
    
    def connectToEngine(self):
        """function to initializate engine"""
        self.systemmonitor = self.dataEngine("systemmonitor")
        if (self.cpuBool):
            self.systemmonitor.connectSource("cpu/system/TotalLoad", self, self.interval)
        if (self.netBool):
            self.systemmonitor.connectSource("network/interfaces/"+self.netdev+"/transmitter/data", self, self.interval)
            self.systemmonitor.connectSource("network/interfaces/"+self.netdev+"/receiver/data", self, self.interval)
        if (self.tempBool):
            self.systemmonitor.connectSource(self.tempdev, self, self.interval)
        if (self.memBool):
            if (self.memInMb):
                self.systemmonitor.connectSource("mem/physical/application", self, self.interval)
            else:
                self.systemmonitor.connectSource("mem/physical/free", self, 200)
                self.systemmonitor.connectSource("mem/physical/used", self, 200)
                self.systemmonitor.connectSource("mem/physical/application", self, 200)
        if (self.swapBool):
            if (self.swapInMb):
                self.systemmonitor.connectSource("mem/swap/used", self, self.interval)
            else:
                self.systemmonitor.connectSource("mem/swap/free", self, 200)
                self.systemmonitor.connectSource("mem/swap/used", self, 200)
    
    @pyqtSignature("dataUpdated(const QString &, const Plasma::DataEngine::Data &)")
    def dataUpdated(self, sourceName, data):
        """function to refresh data"""
        if (sourceName == "cpu/system/TotalLoad"):
            value = str(round(float(data[QString(u'value')]), 1))
            cpuText = "%5s" % (value)
            line = self.cpuFormat.split('$cpu')[0] + cpuText + self.cpuFormat.split('$cpu')[1]
            text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
            self.label_cpu.setText(text)
        elif (sourceName == "network/interfaces/"+self.netdev+"/transmitter/data"):
            value = str(data[QString(u'value')]).split('.')[0]
            up_speed = "%4s" % (value)
            line = '/' + up_speed + self.netFormat.split('$net')[1]
            text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
            self.label_netUp.setText(text)
        elif (sourceName == "network/interfaces/"+self.netdev+"/receiver/data"):
            value = str(data[QString(u'value')]).split('.')[0]
            down_speed = "%4s" % (value)
            line = self.netFormat.split('$net')[0] + down_speed
            text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
            self.label_netDown.setText(text)
        elif (sourceName == self.tempdev):
            value = str(round(float(data[QString(u'value')]), 1))
            tempText = "%4s" % (value)
            line = self.tempFormat.split('$temp')[0] + tempText + self.tempFormat.split('$temp')[1]
            text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
            self.label_temp.setText(text)
        elif (sourceName == "mem/physical/free"):
            self.mem_free = float(data[QString(u'value')])
        elif (sourceName == "mem/physical/used"):
            self.mem_uf = float(data[QString(u'value')])
        elif (sourceName == "mem/physical/application"):
            if (self.memInMb):
                mem = str(round(float(data[QString(u'value')]) / 1024, 0)).split('.')[0]
                mem = "%5s" % (mem)
                line = self.memFormat.split('$memmb')[0] + mem + self.memFormat.split('$memmb')[1]
                text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                self.label_mem.setText(text)
            else:
                self.mem_used = float(data[QString(u'value')])
        elif (sourceName == "mem/swap/free"):
            self.swap_free = float(data[QString(u'value')])
        elif (sourceName == "mem/swap/used"):
            if (self.swapInMb):
                mem = str(round(float(data[QString(u'value')]) / 1024, 0)).split('.')[0]
                mem = "%5s" % (mem)
                line = self.swapFormat.split('$swapmb')[0] + mem + self.swapFormat.split('$swapmb')[1]
                text = self.formatLine.split('$LINE')[0] + line + self.formatLine.split('$LINE')[1]
                self.label_swap.setText(text)
            else:
                self.swap_used = float(data[QString(u'value')])
        


def CreateApplet(parent):
	return pyTextWidget(parent)