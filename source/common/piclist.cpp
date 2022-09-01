/*****************************************************************************
 * Copyright (C) 2013-2020 MulticoreWare, Inc
 *
 * Authors: Gopu Govindaswamy <gopu@multicorewareinc.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at license @ x265.com.
 *****************************************************************************/

#include "common.h"
#include "piclist.h"
#include "frame.h"

using namespace X265_NS;


//链表相关说明：
//PicList链表类有m_start和m_end两个成员（Frame类），分别指向链表的第一个元素和最后一个元素
//每一帧数据（Frame类）都有一个指向前（m_pre）和指向后（m_next）的指针

//往链表中插入一帧数据（链表头）
void PicList::pushFront(Frame& curFrame)
{
    //确保当前帧不在链表中
    X265_CHECK(!curFrame.m_next && !curFrame.m_prev, "piclist: picture already in list\n"); // ensure frame is not in a list
    //给当前帧指针的m_pre和m_next赋值
    curFrame.m_next = m_start;
    curFrame.m_prev = NULL;

    if (m_count)  //如果链表之前就有帧存在
    {
        m_start->m_prev = &curFrame;   //第一帧的m_prev指向当前帧
        m_start = &curFrame;           //m_start指向当前帧
    }
    else  //如果链表中没有帧
    {
        m_start = m_end = &curFrame;
    }
    m_count++;
}

//往链表中插入一帧数据（链表尾）
void PicList::pushBack(Frame& curFrame)
{
    //确保当前帧不在链表中
    X265_CHECK(!curFrame.m_next && !curFrame.m_prev, "piclist: picture already in list\n"); // ensure frame is not in a list
    //给当前帧指针的m_pre和m_next赋值
    curFrame.m_next = NULL;  //链表最后一个元素指向null
    curFrame.m_prev = m_end;

    if (m_count)  //如果链表之前就有帧存在
    {
        m_end->m_next = &curFrame;  //最后一个帧（m_end）的next指向当前帧
        m_end = &curFrame;          //当前帧是最后一个元素（m_end）
    }
    else   //如果链表之前没有帧，说明当前帧是链表第一个元素
    {
        m_start = m_end = &curFrame;  //m_start和m_end都指向该元素
    }
    m_count++;   //链表元素累加
}

//删除链表中的第一帧（链表头）并返回
Frame *PicList::popFront()
{
    if (m_start)  //如果链表中有数据
    {
        Frame *temp = m_start;
        m_count--;

        if (m_count) //不止一帧
        {
            m_start = m_start->m_next;  //m_start指向下一帧
            m_start->m_prev = NULL;
        }
        else  //链表中只有一帧
        {
            m_start = m_end = NULL;  //头尾指针赋值null
        }
        temp->m_next = temp->m_prev = NULL;  //给当前帧指针的m_pre和m_next赋值(从链表中抽离出一帧)
        return temp;
    }
    else
        return NULL;
}

Frame* PicList::getPOC(int poc)
{
    Frame *curFrame = m_start;
    while (curFrame && curFrame->m_poc != poc)
        curFrame = curFrame->m_next;
    return curFrame;
}

//删除链表中的最后一帧（链表尾）并返回
Frame *PicList::popBack()
{
    //删除链表尾的前提是m_end存在
    if (m_end)
    {
        Frame* temp = m_end;
        m_count--;

        if (m_count)  //链表中不止一帧数据
        {
            m_end = m_end->m_prev;
            m_end->m_next = NULL;
        }
        else  //链表中只有这一帧
        {
            m_start = m_end = NULL;
        }
        temp->m_next = temp->m_prev = NULL;  //给当前帧指针的m_pre和m_next赋值(从链表中抽离出一帧)
        return temp;
    }
    else
        return NULL;
}

//获取链表中的第一帧（注意：是指向了m_start，并没有删除！！！）
Frame* PicList::getCurFrame(void)
{
    Frame *curFrame = m_start;
    if (curFrame != NULL)
        return curFrame;
    else
        return NULL;
}

void PicList::remove(Frame& curFrame)
{
#if _DEBUG
    Frame *tmp = m_start;
    while (tmp && tmp != &curFrame)
    {
        tmp = tmp->m_next;
    }

    X265_CHECK(tmp == &curFrame, "piclist: pic being removed was not in list\n"); // verify pic is in this list
#endif

    m_count--;
    if (m_count)
    {
        if (m_start == &curFrame)
            m_start = curFrame.m_next;
        if (m_end == &curFrame)
            m_end = curFrame.m_prev;

        if (curFrame.m_next)
            curFrame.m_next->m_prev = curFrame.m_prev;
        if (curFrame.m_prev)
            curFrame.m_prev->m_next = curFrame.m_next;
    }
    else
    {
        m_start = m_end = NULL;
    }

    curFrame.m_next = curFrame.m_prev = NULL;
}
