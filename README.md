#Ethereum CLL Contract Simulator

## examples


assign value

```
example_value = 10
```

create array

```
example_array = array(10)
```


access array elements

```
exampel_array = array(10)
example_array[7] = 5
example_value = example_array[7]
```

values operations
```
1 * 2 - 5 - (9 * -5) % 3
example_value = 5 -3 * 2 * (9 -2)
```


if

```
if tx.value > block.basefee * 200:
    example_value = 10
end
```


while

```
i = 0
while i < 5:
    i = i + 1
end
```

stop

```
if tx.value < block.basefee * 200:
    stop
end
```

