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


//�������˵����
//PicList��������m_start��m_end������Ա��Frame�ࣩ���ֱ�ָ������ĵ�һ��Ԫ�غ����һ��Ԫ��
//ÿһ֡���ݣ�Frame�ࣩ����һ��ָ��ǰ��m_pre����ָ���m_next����ָ��

//�������в���һ֡���ݣ�����ͷ��
void PicList::pushFront(Frame& curFrame)
{
    //ȷ����ǰ֡����������
    X265_CHECK(!curFrame.m_next && !curFrame.m_prev, "piclist: picture already in list\n"); // ensure frame is not in a list
    //����ǰָ֡���m_pre��m_next��ֵ
    curFrame.m_next = m_start;
    curFrame.m_prev = NULL;

    if (m_count)  //�������֮ǰ����֡����
    {
        m_start->m_prev = &curFrame;   //��һ֡��m_prevָ��ǰ֡
        m_start = &curFrame;           //m_startָ��ǰ֡
    }
    else  //���������û��֡
    {
        m_start = m_end = &curFrame;
    }
    m_count++;
}

//�������в���һ֡���ݣ�����β��
void PicList::pushBack(Frame& curFrame)
{
    //ȷ����ǰ֡����������
    X265_CHECK(!curFrame.m_next && !curFrame.m_prev, "piclist: picture already in list\n"); // ensure frame is not in a list
    //����ǰָ֡���m_pre��m_next��ֵ
    curFrame.m_next = NULL;  //�������һ��Ԫ��ָ��null
    curFrame.m_prev = m_end;

    if (m_count)  //�������֮ǰ����֡����
    {
        m_end->m_next = &curFrame;  //���һ��֡��m_end����nextָ��ǰ֡
        m_end = &curFrame;          //��ǰ֡�����һ��Ԫ�أ�m_end��
    }
    else   //�������֮ǰû��֡��˵����ǰ֡�������һ��Ԫ��
    {
        m_start = m_end = &curFrame;  //m_start��m_end��ָ���Ԫ��
    }
    m_count++;   //����Ԫ���ۼ�
}

//ɾ�������еĵ�һ֡������ͷ��������
Frame *PicList::popFront()
{
    if (m_start)  //���������������
    {
        Frame *temp = m_start;
        m_count--;

        if (m_count) //��ֹһ֡
        {
            m_start = m_start->m_next;  //m_startָ����һ֡
            m_start->m_prev = NULL;
        }
        else  //������ֻ��һ֡
        {
            m_start = m_end = NULL;  //ͷβָ�븳ֵnull
        }
        temp->m_next = temp->m_prev = NULL;  //����ǰָ֡���m_pre��m_next��ֵ(�������г����һ֡)
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

//ɾ�������е����һ֡������β��������
Frame *PicList::popBack()
{
    //ɾ������β��ǰ����m_end����
    if (m_end)
    {
        Frame* temp = m_end;
        m_count--;

        if (m_count)  //�����в�ֹһ֡����
        {
            m_end = m_end->m_prev;
            m_end->m_next = NULL;
        }
        else  //������ֻ����һ֡
        {
            m_start = m_end = NULL;
        }
        temp->m_next = temp->m_prev = NULL;  //����ǰָ֡���m_pre��m_next��ֵ(�������г����һ֡)
        return temp;
    }
    else
        return NULL;
}

//��ȡ�����еĵ�һ֡��ע�⣺��ָ����m_start����û��ɾ����������
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
