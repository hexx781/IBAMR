%% ---------------------------------------------------------------------
%%
%% Copyright (c) 2014 - 2014 by the IBAMR developers
%% All rights reserved.
%%
%% This file is part of IBAMR.
%%
%% IBAMR is free software and is distributed under the 3-clause BSD
%% license. The full text of the license can be found in the file
%% COPYRIGHT at the top level directory of IBAMR.
%%
%% ---------------------------------------------------------------------

function n = L2norm(p)
   N1 = size(p,1);
   N2 = size(p,2);
   if (N1 ~= N2)
      error('invalid grid spacing')
   end
   h = 1/N1;
   n = sqrt(sum(sum(p.^2))*h^2);
