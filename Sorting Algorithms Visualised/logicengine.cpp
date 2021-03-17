#include "logicengine.h"

LogicEngine::LogicEngine()
{
    initValues(300);
}

void LogicEngine::setUi(GUI *ui)
{
    ui_ = ui;
    ui->initBars(&values_);
    connect(ui, &GUI::wannaSort, this, &LogicEngine::beginSorting);
    connect(ui, &GUI::shuffle, this, &LogicEngine::shuffleBars);
}

void LogicEngine::bubbleSort()
{
    int swaps = 1;
    while(swaps != 0) {
        swaps = 0;
        for(int i = 0; i < 199; i++) {
            if(values_.at(i) > values_.at(i+1)) {
                int val = values_.at(i+1);
                values_.at(i+1) = values_.at(i);
                values_.at(i) = val;

                ui_->swap(i, i+1);
                swaps++;
            }
        }
    }
}

void LogicEngine::mergeSort()
{
    std::map<int, std::vector<int>> parts;
    int idx = 0;
    for(int val : values_) {
        parts.insert({idx, {val}});
        idx++;
    }

    while(parts.size() > 1) {
        for(auto it = parts.begin(); it != parts.end();) {
            auto it2 = it;
            it2++;
            std::vector<int> left = it->second;
            std::vector<int> right;
            if(it2 == parts.end()) {
                break;
            } else {
                right = it2->second;
            }
            std::vector<int> results = {};

            int nL = left.size();
            int nR = right.size();
            int i = 0, j = 0, k = 0;

            while (j < nL && k < nR)
            {
                if (left.at(j) < right.at(k)) {
                    results.push_back(left.at(j));
                    j++;
                }
                else {
                    results.push_back(right.at(k));
                    k++;
                }
                i++;
            }
            while (j < nL) {
                results.push_back(left.at(j));
                j++; i++;
            }
            while (k < nR) {
                results.push_back(right.at(k));
                k++; i++;
            }

            int a = it->first;
            for(int val : results) {
                values_.at(a) = val;
                ui_->setValue(a, val);
                a++;
            }

            it->second = results;
            if(it2 != parts.end()) {
                parts.erase(it2);
            }
            it++;

        }
    }
}

void LogicEngine::quickSort(int min, int max)
{
    if(max-min == 2) {
        if(values_.at(min) > values_.at(min+1)) {
            swapValues(min, min+1);
            ui_->swap(min, min+1);
        }
        return;
    }
    if(max-min < 2) {
        return;
    }

    int pivot = min+((max-min-1)/2);
    int pivval = values_.at(pivot);

    int lastval = values_.at(max-1);
    values_.at(pivot) = lastval;
    values_.at(max-1) = pivval;
    ui_->swap(pivot, max-1); // put pivot at the back
    pivot = max-1;

    int i = 0;
    while(i < pivot) {
        if(values_.at(i) > pivval) {
            values_.at(pivot) = values_.at(i);
            values_.at(i) = values_.at(pivot-1);
            values_.at(pivot-1) = pivval;
            ui_->swap(i, pivot);
            ui_->swap(i, pivot-1);
            pivot--;
            i--;
        }
        i++;
    }
    quickSort(min, pivot);
    quickSort(pivot, max);
}

void LogicEngine::radixSort() // Sorted container mustn't contain more than 3-digit values!!!
{
    std::vector<std::vector<int>> workingCont1 = {{},{},{},{},{},{},{},{},{},{},};
    std::vector<std::vector<int>> workingCont2 = {{},{},{},{},{},{},{},{},{},{},};

    for(int val : values_) {
        int idx = getDigit(val, 0);
        workingCont1.at(idx).push_back(val);
    }
    int a = 0;
    for(auto& vec : workingCont1) {
        for(int val : vec) {
            values_.at(a) = val;
            ui_->setValue(a, val);
            a++;
        }
    }

    for(int i = 0; i < 10; i++) {
        for(int val : workingCont1.at(i)) {
            int idx = getDigit(val, 1);
            workingCont2.at(idx).push_back(val);
        }
    }
    a = 0;
    for(auto& vec : workingCont2) {
        for(int val : vec) {
            values_.at(a) = val;
            ui_->setValue(a, val);
            a++;
        }
    }
    workingCont1 = {{},{},{},{},{},{},{},{},{},{},};

    for(int i = 0; i < 10; i++) {
        for(int val : workingCont2.at(i)) {
            int idx = getDigit(val, 2);
            workingCont1.at(idx).push_back(val);
        }
    }
    a = 0;
    for(auto& vec : workingCont1) {
        for(int val : vec) {
            values_.at(a) = val;
            ui_->setValue(a, val);
            a++;
        }
    }


}

void LogicEngine::swapValues(int idx1, int idx2)
{
    int val = values_.at(idx1);
    values_.at(idx1) = values_.at(idx2);
    values_.at(idx2) = val;
}

int LogicEngine::getDigit(int num, int n)
{
    for(int i = 0; i < n; i++) {
        num = num/10;
    }

    return num%10;

}

void LogicEngine::beginSorting(QString algorithm)
{
    if(algorithm == "Bubble sort") {
        bubbleSort();

    } else if(algorithm == "Merge sort") {
        mergeSort();

    } else if(algorithm == "Quicksort") {
        quickSort(0, values_.size());

    } else if(algorithm == "Radix sort") {
        radixSort();
    }

    ui_->checkSorted();
}

void LogicEngine::shuffleBars(int count)
{
    initValues(count);
    ui_->initBars(&values_);
}

void LogicEngine::initValues(int amount)
{
    values_ = {};

    float step = static_cast<float>(300)/static_cast<float>(amount);

    float j = 50.0;
    while(j < 350) {
        int i = j;
        values_.push_back(i);
        j += step;
    }
    std::random_shuffle(values_.begin(), values_.end());
}
