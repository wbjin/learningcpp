# std::accumulate

Takes in a begin iterator, end iterator, an initial accumulation value, and
optionally a binary operator. For example, in order to accumulate strings with
a particular condition (no characters that appear three times in a row):
```
return std::accumulate(s.begin(), s.end(), std::string{}, [](std::string acc, char c){
    std::size_t len = acc.size();
    if (len >= 2 && acc[len-1] == c && acc[len-2] == c )
        return acc;
    acc+=c;
    return acc;
});
```

The binary operator takes in the accumulation variable type and the type of the
elements being accumulated.
