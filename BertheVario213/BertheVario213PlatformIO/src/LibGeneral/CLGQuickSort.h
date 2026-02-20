//////////////////////////////////////////////////////////////////////////////
/// \file CLGQuickSort.h
/// \date 31/01/2019 : date de creation
/// \addtogroup LibGeneral
/// \brief CLGQuickSortArr et CLGQuickSortArrp
///
/// \date 19/10/2020 : NVIDIA_FUNC
/// \date 14/06/2025 : On enleve les NVIDIA_FUNC.
/// \date 14/06/2025 : Derniere modification.
///

///////////////////////////////////////////////////////////////////////////////
/// \brief Tableau QuickSort
template< class T>
class CLGQuickSortArr
{
protected :
    void quickSort(T arr[], int low, int high, bool TriCroissant )  ;
private :
    void swap(T* a, T* b)  ;
    int partition(T arr[], int low, int high, bool TriCroissant) ;
} ;


///////////////////////////////////////////////////////////////////////////////
/// \brief A utility function to swap two elements
template< class T>
void CLGQuickSortArr<T>::swap(T* a, T* b)
{
T t = *a;
*a = *b;
*b = t;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief This function takes last element as pivot, places
/// the pivot element at its correct position in sorted
/// array, and places all smaller (smaller than pivot)
/// to left of pivot and all greater elements to right
/// of pivot
///
template< class T>
int CLGQuickSortArr<T>::partition(T arr[], int low, int high, bool TriCroissant)
{
T pivot = arr[high];    // pivot
int i = (low - 1);  // Index of smaller element

if ( TriCroissant )
    {
    for (int j = low; j <= high- 1; j++)
        {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot)
            {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
            }
        }
    }
else
    {
    for (int j = low; j <= high- 1; j++)
        {
        if (arr[j] >= pivot)
            {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
            }
        }
    }
swap(&arr[i + 1], &arr[high]);
return (i + 1);
}


///////////////////////////////////////////////////////////////////////////////
/// \brief The main function that implements QuickSort
/// \param arr[] --> Array to be sorted,
/// \param low  --> Starting index,
/// \param high  --> Ending index
///
template< class T>
void CLGQuickSortArr<T>::quickSort(T arr[], int low, int high, bool TriCroissant)
{
if (low < high)
    {
    /* pi is partitioning index, arr[p] is now
    at right place */
    int pi = partition(arr, low, high, TriCroissant);

    // Separately sort elements before
    // partition and after partition
    quickSort(arr, low, pi - 1, TriCroissant);
    quickSort(arr, pi + 1, high, TriCroissant);
    }
}

/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// \brief Tableau QuickSort
template< class T>
class CLGQuickSortArrp
{
protected :
    void quickSort(T* arr[], int low, int high, bool TriCroissant)  ;
    //void quickSortForDichotomyIndex(T* arr[], int low, int high)  ;
    //void quickSortForDichotomyValue(T* arr[], int low, int high)  ;
    //void quickSortForDichotomyString(T* arr[], int low, int high)  ;
private :
    void swap(T** a, T** b)  ;
    int partition(T* arr[], int low, int high, bool TriCroissant) ;
    //int partitionForDichotomyIndex(T* arr[], int low, int high) ;
    //int partitionForDichotomyValue(T* arr[], int low, int high) ;
    //int partitionForDichotomyString(T* arr[], int low, int high) ;
} ;


///////////////////////////////////////////////////////////////////////////////
/// \brief A utility function to swap two elements
template< class T>
void CLGQuickSortArrp<T>::swap(T** a, T** b)
{
T* t = *a;
*a = *b;
*b = t;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief This function takes last element as pivot, places
/// the pivot element at its correct position in sorted
/// array, and places all smaller (smaller than pivot)
/// to left of pivot and all greater elements to right
/// of pivot
///
template< class T>
int CLGQuickSortArrp<T>::partition(T* arr[], int low, int high, bool TriCroissant)
{
T* pivot = arr[high];    // pivot
int i = (low - 1);  // Index of smaller element

if ( TriCroissant )
    {
    for (int j = low; j <= high- 1; j++)
        {
        // If current element is smaller than or
        // equal to pivot
        if (*arr[j] <= *pivot)
            {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
            }
        }
    }
else
    {
    for (int j = low; j <= high- 1; j++)
        {
        if (*arr[j] >= *pivot)
            {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
            }
        }
    }
swap(&arr[i + 1], &arr[high]);
return (i + 1);
}

/*
///////////////////////////////////////////////////////////////////////////////
/// \brief This function takes last element as pivot, places
/// the pivot element at its correct position in sorted
/// array, and places all smaller (smaller than pivot)
/// to left of pivot and all greater elements to right
/// of pivot
///
template< class T>
int CLGQuickSortArrp<T>::partitionForDichotomyIndex(T* arr[], int low, int high)
{
T* pivot = arr[high];    // pivot
int i = (low - 1);  // Index of smaller element

for (int j = low; j <= high- 1; j++)
    {
    // If current element is smaller than or
    // equal to pivot
    if ( arr[j]->GetDichotomyIndex() <= pivot->GetDichotomyIndex() )
        {
        i++;    // increment index of smaller element
        swap(&arr[i], &arr[j]);
        }
    }
swap(&arr[i + 1], &arr[high]);
return (i + 1);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief This function takes last element as pivot, places
/// the pivot element at its correct position in sorted
/// array, and places all smaller (smaller than pivot)
/// to left of pivot and all greater elements to right
/// of pivot
///
template< class T>
int CLGQuickSortArrp<T>::partitionForDichotomyValue(T* arr[], int low, int high)
{
T* pivot = arr[high];    // pivot
int i = (low - 1);  // Index of smaller element

for (int j = low; j <= high- 1; j++)
    {
    // If current element is smaller than or
    // equal to pivot
    if ( arr[j]->GetDichotomyValue() <= pivot->GetDichotomyValue() )
        {
        i++;    // increment index of smaller element
        swap(&arr[i], &arr[j]);
        }
    }
swap(&arr[i + 1], &arr[high]);
return (i + 1);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief This function takes last element as pivot, places
/// the pivot element at its correct position in sorted
/// array, and places all smaller (smaller than pivot)
/// to left of pivot and all greater elements to right
/// of pivot
///
template< class T>
int CLGQuickSortArrp<T>::partitionForDichotomyString(T* arr[], int low, int high)
{
T* pivot = arr[high];    // pivot
int i = (low - 1);  // Index of smaller element

for (int j = low; j <= high- 1; j++)
    {
    // If current element is smaller than or
    // equal to pivot
    const char * p1 = arr[j]->GetDichotomyString() ;
    const char * p2 = pivot->GetDichotomyString() ;
    //if ( arr[j]->GetDichotomyString() <= pivot->GetDichotomyString() )
    if ( strcmp(p1,p2) <= 0 )
        {
        i++;    // increment index of smaller element
        swap(&arr[i], &arr[j]);
        }
    }
swap(&arr[i + 1], &arr[high]);
return (i + 1);
}
*/

///////////////////////////////////////////////////////////////////////////////
/// \brief The main function that implements QuickSort
/// \param arr[] --> Array to be sorted,
/// \param low  --> Starting index,
/// \param high  --> Ending index
///
template< class T>
void CLGQuickSortArrp<T>::quickSort(T* arr[], int low, int high, bool TriCroissant)
{
if (low < high)
    {
    /* pi is partitioning index, arr[p] is now
    at right place */
    int pi = partition(arr, low, high, TriCroissant);

    // Separately sort elements before
    // partition and after partition
    quickSort(arr, low, pi - 1, TriCroissant);
    quickSort(arr, pi + 1, high, TriCroissant);
    }
}

/*
///////////////////////////////////////////////////////////////////////////////
/// \brief The main function that implements QuickSort
/// \param arr[] --> Array to be sorted,
/// \param low  --> Starting index,
/// \param high  --> Ending index
///
template< class T>
void CLGQuickSortArrp<T>::quickSortForDichotomyIndex(T* arr[], int low, int high)
{
if (low < high)
    {
    // pi is partitioning index, arr[p] is now
    //at right place
    int pi = partitionForDichotomyIndex(arr, low, high);

    // Separately sort elements before
    // partition and after partition
    quickSortForDichotomyIndex(arr, low, pi - 1);
    quickSortForDichotomyIndex(arr, pi + 1, high);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief The main function that implements QuickSort
/// \param arr[] --> Array to be sorted,
/// \param low  --> Starting index,
/// \param high  --> Ending index
///
template< class T>
void CLGQuickSortArrp<T>::quickSortForDichotomyValue(T* arr[], int low, int high)
{
if (low < high)
    {
    // pi is partitioning index, arr[p] is now
    //at right place
    int pi = partitionForDichotomyValue(arr, low, high);

    // Separately sort elements before
    // partition and after partition
    quickSortForDichotomyValue(arr, low, pi - 1);
    quickSortForDichotomyValue(arr, pi + 1, high);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief The main function that implements QuickSort
/// \param arr[] --> Array to be sorted,
/// \param low  --> Starting index,
/// \param high  --> Ending index
///
template< class T>
void CLGQuickSortArrp<T>::quickSortForDichotomyString(T* arr[], int low, int high)
{
if (low < high)
    {
    // pi is partitioning index, arr[p] is now
    // at right place
    int pi = partitionForDichotomyString(arr, low, high);

    // Separately sort elements before
    // partition and after partition
    quickSortForDichotomyString(arr, low, pi - 1);
    quickSortForDichotomyString(arr, pi + 1, high);
    }
}
*/
