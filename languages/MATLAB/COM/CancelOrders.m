function CancelOrders(td, tbl)

[m,n] = size(tbl);

for i=1:m
    if tbl.LeavesQty(i) > 0
        td.CancelOrder(tbl.id{i})
    end
end

end