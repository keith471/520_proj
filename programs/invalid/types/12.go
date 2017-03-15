package noice

func main() {
    var y float64 = 2.0
    switch x := 3; x {
        case 0, 2:
            x++
            break;
        case 1:
            x--
            break;
        case y: // can only switch on an int
            x += y
            break;
        default:
            x += 3
            break;
    }
}
