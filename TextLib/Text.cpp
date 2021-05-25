#include "Text_Iter.h"
#include <fstream>
#include <cstring>

Text::Text(char* s)
{
    root = new TextNode(s, 1);
}

Text::~Text()
{
    TextNode::FreeMem();
}

TextIter Text::GetRoot()
{
    return TextIter(*this, root, TStack<TextNode*>());
}

TextIter Text::Find(char c)
{
    TextIter iter = TextIter(*this, root, TStack<TextNode*>());
        while (iter.GoNextChar())
        {
            if (iter.Get()->GetC() == c)
                return iter;
        }
    return iter;
}

TextIter Text::FindWord(char* c)
{
    TextIter iter = GetRoot();

    while (iter.GoNext())
    {
            TextIter result = iter;
            bool word = true;
            int i = 0;
            int n = strlen(c);

            while (iter.GoNext() && i < n)
            {

                if (iter.Get()->GetC() != c[i])
                {
                    word = false;
                    break;
                }
                i++;
            }

            if (word)
                return result;
    }
    
    return iter;      

}

TextIter Text::Find(char* s)
{
    TextIter iter = GetRoot();

    while (iter.GoNextChar())
    {
        TextIter result = iter;
        bool letter = true;
        bool next = true;
        int n = strlen(s);

        for (int i = 0; i < n; i++)
        {
            if (s[i] != result.Get()->GetC()) {
                letter = false;
                break;
            }
            next = result.GoNextChar();
        }

        if (letter)
            return iter;
    }
}


void Text::Insert(char* c, TextIter iter)
{
    TextNode* node = new TextNode(c, iter.Get()->GetLevel());
    TextNode* next = iter.Get()->GetNext();
    node->SetNext(next);
    iter.Get()->SetNext(node);
}

void Text::InsertData(char* c, TextIter iter)
{

    if (iter.Get()->GetNext()->GetC() == ' ' && iter.Get()->GetNext()->GetC() == '.' && iter.Get()->GetNext()->GetC() == '\n')
    {
        iter.GoNext();
        TextNode* node = new TextNode(c, 2);
        iter.Get()->SetNext(node);

        int k = strlen(c) + 1;
        for (int i = 0; i < k; i++)
            iter.GoNext();

        node = new TextNode(" ", 2);
        iter.Get()->SetNext(node);
    }

    else
    {
        TextNode* next = iter.Get()->GetNext();
        TextNode* tmp = iter.Get()->GetNext();
        int w = 0;

        while (next->GetC() != ' ' && next->GetC() != '.' && next->GetC() != '\n')
        {
            w++;
            next = next->GetNext();
        }

        char* word = new char[w + 2];
        word[0] = ' ';
        word[w + 2] = NULL;

        for (int i = 1; i < w + 2; i++)
        {
            word[i] = tmp->GetC();
            tmp = tmp->GetNext();
        }

        TextNode* node = new TextNode(" ", 2);
        iter.Get()->SetNext(node);

        iter.GoNext();
        iter.GoNext();
        node = new TextNode(c, 2);
        iter.Get()->SetNext(node);

        int k = strlen(c) + 1;
        for (int i = 0; i < k; i++)
            iter.GoNext();

        node = new TextNode(word, 2);
        iter.Get()->SetNext(node);
    }
           
         
}

void Text::InsertLine(char* c, TextIter iter)
{
    while (iter.Get()->GetC() != '\n')
    {
        iter.GoNext();
    }

    TextNode* node = new TextNode(c, 1);
    iter.Get()->SetNext(node);

    iter.GoNext();
    node = new TextNode("\n", 2);
    iter.Get()->SetNext(node);
}


void Text::Delete(int count, TextIter iter)
{
    if (count == 0)
        return;

    TStack<TextNode*> stack[3];
    int del = 0;
    int level = iter.Get()->GetLevel();

    if (level < 3)
        iter.GoNextChar();

    if (level >= 1 && level <= 3)
    {
        stack[level - 1].Push(iter.Get());
        if (level == 3)
            del++;
    }

    TextIter it = iter;
    while (it.GoNext() && del < count)
    {
        level = it.Get()->GetLevel();

        if (level >= 1 && level <= 3)
        {
            stack[level - 1].Push(it.Get());

            if (level == 3)
                del++;
        }
    }

    if (del < count)
        throw logic_error("ERROR");

    if (stack[0].IsEmpty() && stack[1].IsEmpty())
    {
        TextIter prev = iter;
        prev.ResetToWord();
        while (prev.Get()->GetNext() != iter.Get() && prev.Get()->GetDown() != iter.Get())
            prev.GoNext();

        TextNode* last = stack[2].Get();

        if (prev.Get()->GetLevel() == 2)
            prev.Get()->SetDown(last->GetNext());
        else
            prev.Get()->SetNext(last->GetNext());

        delete last;
        return;
    }


    TextIter prev = iter;
    prev.ResetToWord();

    while (prev.Get()->GetNext() != iter.Get() && prev.Get()->GetDown() != iter.Get())
        prev.GoNext();

    if (prev.Get()->GetNext() == iter.Get())
        prev.Get()->SetNext(nullptr);

    else
        prev.Get()->SetDown(nullptr);

    if (stack[0].IsEmpty()) 
    {
        TextNode* last_word = stack[1].Get();
        TextNode* last_letter = stack[2].Get();
        TextIter cur = iter;
        cur.ResetToWord();
        cur.Get()->SetNext(last_word);
        last_word->SetDown(last_letter->GetNext());
        delete last_letter;

        for (int i = 1; i < 3; i++)
        {
            while (!stack[i].IsEmpty())
            {
                TextNode* del = stack[i].Get();
                delete del;
            }
        }
    }
    else 
    {
        TextNode* last_string = stack[0].Get();
        TextNode* last_word = stack[1].Get();
        TextNode* last_letter = stack[2].Get();
        TextIter cur = iter;
        cur.ResetToString();
        cur.Get()->SetNext(last_string);
        cur.ResetToWord();
        cur.Get()->SetNext(last_word);
        last_word->SetDown(last_letter->GetNext());
        delete last_letter;

        for (int i = 0; i < 3; i++)
        {
            while (!stack[i].IsEmpty())
            {
                TextNode* temp = stack[i].Get();
                delete temp;
            }
        }
    }
}

char* Text::Copy(int count, TextIter iter)
{
    if (count < 0)
        throw logic_error("Error");

    char* res = new char[count ++];

    while (iter.Get()->GetLevel() < 3)
        iter.GoNext();

    if (count > 0)
    {
        res[0] = iter.Get()->GetC();

        for (int i = 0; i < count; i++)
        {
            iter.GoNextChar();
            res[i] = iter.Get()->GetC();

        }
    }

    res[count] = 0;
    return res;
}

std::ostream& operator<<(std::ostream& out, Text& t)
{
    out << *(t.root);
    return out;
}
