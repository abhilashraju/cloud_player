#pragma once
#include "qcomponents.hpp"
#define BEGIN_LAYOUT(x) \
    {                   \
        x, [](QItemWrapper  view, std::map<std::string, QQuickItem *> & objMap)
#define END_LAYOUT \
    }              \
    ,
using LAYOUT_MAP = std::map<std::string, std::function<void(QItemWrapper , std::map<std::string, QQuickItem *> &)>>;


constexpr int buttonwidth=50;
constexpr int buttonheight=50;
namespace Hp {
    void textview_singlebutton(QItemWrapper screen, QItemWrapper button, QItemWrapper textview)
    {

        
        Hp::anchors(button)["right"]= screen["right"];
        Hp::anchors(button)["verticalCenter"]= screen["verticalCenter"];
        button->setWidth(buttonwidth);
        button->setHeight(buttonheight);
        Hp::anchors(textview)["top"]= screen["top"];
        Hp::anchors(textview)["left"]= screen["left"];
        Hp::anchors(textview)["bottom"]= screen["bottom"];
        Hp::anchors(textview)["right"]= button["left"];

    }
    void list_with_single_button(QItemWrapper screen, QItemWrapper button, QItemWrapper listview)
    {
        textview_singlebutton(screen,button,listview);
    }
    void textview_twobutton(QItemWrapper screen, QItemWrapper button1, QItemWrapper button2,QItemWrapper textview)
    {
        button1["color"] = QVariant(QColor(125,125,125));
        button1["icon"] = QVariant(":/images/button.json");
        
        Hp::anchors(textview)["top"]= screen["top"];
        Hp::anchors(textview)["left"]= screen["left"];
        Hp::anchors(textview)["bottom"]= screen["bottom"];
        Hp::anchors(textview)["right"]= button1["left"];
        Hp::anchors(button1)["right"]= screen["right"];
        Hp::anchors(button1)["top"]= screen["top"];
        button1->setWidth(buttonwidth);
        button1->setHeight(buttonheight);
        Hp::anchors(button2)["left"]= button1["left"];
        Hp::anchors(button2)["right"]= button1["right"];
        Hp::anchors(button2)["top"]= button1["bottom"];
        Hp::anchors(button2)["topMargin"]= 20;
    }
    void list_with_two_buttons(QItemWrapper screen, QItemWrapper button1, QItemWrapper button2,QItemWrapper textview)
    {
        textview_twobutton(screen,button1,button2,textview);
    }
    void textview_fullscreen(QItemWrapper screen,QItemWrapper textview)
    {

        
        Hp::anchors(textview)["top"]= screen["top"];
        Hp::anchors(textview)["left"]= screen["left"];
        Hp::anchors(textview)["bottom"]= screen["bottom"];
        Hp::anchors(textview)["right"]= screen["right"];

    }
    void vw_menu(QItemWrapper screen,QItemWrapper list)
    {
        textview_fullscreen(screen,list);
    }
    void icon_text(QItemWrapper screen,QItemWrapper icon,QItemWrapper text)
    {
        
        Hp::anchors(icon)["horizontalCenter"]= screen["horizontalCenter"];
        Hp::anchors(icon)["top"]= screen["top"];
         Hp::anchors(icon)["topMargin"]= 10;
        icon->setWidth(100);
        icon->setHeight(100);
        Hp::anchors(text)["top"]= icon["bottom"];
        Hp::anchors(text)["bottom"]= screen["bottom"];
        Hp::anchors(text)["left"]= screen["left"];
        Hp::anchors(text)["right"]= screen["right"];
    }
    static LAYOUT_MAP &getLayoutMap()
    {
        static LAYOUT_MAP layoutMap = {
    BEGIN_LAYOUT("$B$H$ok"){
        QItemWrapper button{objMap["$ok"]};
        QItemWrapper header{objMap["$H"]};
        QItemWrapper textview{objMap["$B"]};
        header["visible"] = false;
        textview_singlebutton(view, button, textview);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$B$H"){
     
        QItemWrapper header{objMap["$H"]};
        QItemWrapper textview{objMap["$B"]};
        header["visible"] = false;
        textview_fullscreen(view,textview);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$B$ok"){
        QItemWrapper button{objMap["$ok"]};
        QItemWrapper textview{objMap["$B"]};
        textview_singlebutton(view, button, textview);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$I$T"){
        QItemWrapper icon{objMap["$I"]};
        QItemWrapper text{objMap["$T"]};
        icon_text(view, icon, text);
    }
    END_LAYOUT


    BEGIN_LAYOUT("$B$H$ok"){
        QItemWrapper button{objMap["$ok"]};
        QItemWrapper header{objMap["$H"]};
        QItemWrapper textview{objMap["$B"]};
        header["visible"] = false;
        textview_singlebutton(view, button, textview);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$B$ok"){
        QItemWrapper button{objMap["$ok"]};
        QItemWrapper textview{objMap["$B"]};
        textview_singlebutton(view, button, textview);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$B$H$altOk$ok"){
        QItemWrapper button{objMap["$ok"]};
        QItemWrapper header{objMap["$H"]};
        QItemWrapper textview{objMap["$B"]};
        QItemWrapper button2{objMap["$altOk"]};
        header["visible"] = false;
        textview_twobutton(view, button,button2,textview);
    }
    END_LAYOUT


    BEGIN_LAYOUT("$B$altOk$ok"){
        QItemWrapper button{objMap["$ok"]};
        QItemWrapper textview{objMap["$B"]};
        QItemWrapper button2{objMap["$altOk"]};

        textview_twobutton(view, button,button2,textview);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$L$help$ok"){
        QItemWrapper list{objMap["$L"]};
        QItemWrapper buttontop{objMap["$help"]};
        QItemWrapper buttonbtm{objMap["$ok"]};
        buttontop["visible"] = false;
        list_with_single_button(view, buttonbtm,list);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$H$L$altOk$help$ok"){
        QItemWrapper header{objMap["$H"]};
        QItemWrapper list{objMap["$L"]};
        QItemWrapper buttontop{objMap["$altOk"]};
        QItemWrapper buttonbtm{objMap["$ok"]};
        QItemWrapper help{objMap["$help"]};
        header["visible"] = false;
        help["visible"] = false;
        list_with_two_buttons(view,buttontop, buttonbtm,list);
    }
    END_LAYOUT

    BEGIN_LAYOUT("$H$L"){
        QItemWrapper header{objMap["$H"]};
        QItemWrapper list{objMap["$L"]};
        header["visible"] = false;
        vw_menu(view, list);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$L"){
        QItemWrapper list{objMap["$L"]};
        vw_menu(view, list);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$L$help"){
        QItemWrapper list{objMap["$L"]};
        vw_menu(view, list);
    }
    END_LAYOUT

    BEGIN_LAYOUT("$H$L$altOk$context")
    {
        QItemWrapper list{objMap["$L"]};
        QItemWrapper buttontop{objMap["$context"]};
        QItemWrapper buttonbtm{objMap["$altOk"]};
        list_with_two_buttons(view, list, buttontop, buttonbtm);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$L$altOk$context")
    {
        QItemWrapper list{objMap["$L"]};
        QItemWrapper buttontop{objMap["$context"]};
        QItemWrapper buttonbtm{objMap["$altOk"]};
        list_with_two_buttons(view, list, buttontop, buttonbtm);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$H$L$context$ok")
    {
        QItemWrapper list{objMap["$L"]};
        QItemWrapper buttontop{objMap["$context"]};
        QItemWrapper buttonbtm{objMap["$ok"]};
        list_with_two_buttons(view, list, buttontop, buttonbtm);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$L$context$ok")
    {
        QItemWrapper list{objMap["$L"]};
        QItemWrapper buttontop{objMap["$context"]};
        QItemWrapper buttonbtm{objMap["$ok"]};
        list_with_two_buttons(view, list, buttontop, buttonbtm);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$H$L$ok")
    {
        QItemWrapper listmenu{objMap["$L"]};
        QItemWrapper button{objMap["$ok"]};
        if(!listmenu["fullscreen"]().isValid()){
            list_with_single_button(view, button, listmenu);
            return;
        }
        button["visible"]=false;
        vw_menu(view, listmenu);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$L$ok")
    {
        QItemWrapper listmenu{objMap["$L"]};
        QItemWrapper button{objMap["$ok"]};
        list_with_single_button(view, button,listmenu);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$B$H$L$ok")
    {
        QItemWrapper listmenu{objMap["$L"]};
        QItemWrapper button{objMap["$ok"]};
        QItemWrapper body{objMap["$B"]};
        QItemWrapper header{objMap["$H"]};
        body["visible"]=false;
        header["visible"]=false;
        if(!listmenu["fullscreen"]().isValid()){
            list_with_single_button(view, listmenu, button);
            return;
        }
        button["visible"]=false;

        vw_menu(view, listmenu);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$B$L$ok")
    {
        QItemWrapper listmenu{objMap["$L"]};
        QItemWrapper button{objMap["$ok"]};
        QItemWrapper body{objMap["$B"]};
        body["visible"]=false;
        if(!listmenu["fullscreen"]().isValid()){
            list_with_single_button(view, button, listmenu);
            return;
        }
        button["visible"]=false;

        vw_menu(view, listmenu);
    }
    END_LAYOUT

    BEGIN_LAYOUT("$H$L$altOk$context$ok")
    {
        QItemWrapper listmenu{objMap["$L"]};
        QItemWrapper buttontop{objMap["$altOk"]};
        QItemWrapper buttonbtm{objMap["$context"]};
        list_with_two_buttons(view, buttontop, buttonbtm, listmenu);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$L$altOk$context$ok")
    {
        QItemWrapper listmenu{objMap["$L"]};
        QItemWrapper buttontop{objMap["$altOk"]};
        QItemWrapper buttonbtm{objMap["$context"]};
        list_with_two_buttons(view, buttontop, buttonbtm, listmenu);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$A$B$cancel")
    {
        QItemWrapper button{objMap["$cancel"]};
        QItemWrapper textview{objMap["$B"]};
        textview_singlebutton(view, button, textview);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$A$B$H$ok")
    {
        QItemWrapper button{objMap["$ok"]};
        QItemWrapper textview{objMap["$B"]};
        textview_singlebutton(view, button, textview);
    }
    END_LAYOUT


    BEGIN_LAYOUT("$H$L$altOk$ok")
    {
        QItemWrapper listmenu{objMap["$L"]};
        QItemWrapper buttontop{objMap["$altOk"]};
        QItemWrapper buttonbtm{objMap["$ok"]};
        list_with_two_buttons(view, buttontop, buttonbtm, listmenu);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$L$altOk$ok")
    {
        QItemWrapper listmenu{objMap["$L"]};
        QItemWrapper buttontop{objMap["$altOk"]};
        QItemWrapper buttonbtm{objMap["$ok"]};
        list_with_two_buttons(view, buttontop, buttonbtm, listmenu);
    }
    END_LAYOUT
    BEGIN_LAYOUT("$H$ok")
    {
        QItemWrapper button{objMap["$ok"]};
        QItemWrapper header{objMap["$H"]};
        // QItemWrapper textview{objMap["$B"]};
        // header["visible"] = false;
        textview_singlebutton(view, button, header);
    }
    END_LAYOUT


    BEGIN_LAYOUT("$B")
    {
        QItemWrapper message{objMap["$B"]};
        textview_fullscreen(view, message);
    }
    END_LAYOUT


    };

    return layoutMap;
    }
}

