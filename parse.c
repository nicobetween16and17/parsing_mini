#include <readline/readline.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum e_type
{
	REDIR,
	CMD,
	ELEMENT,
	ELEM_S_QUOTES,
	ELEM_D_QUOTES,
	FLAGS,
	FILENAME
}	t_type;

typedef struct s_token
{
	char			*s;
	t_type			genre;
	struct s_token	*next;
}	t_token;

typedef struct s_parsing
{
	t_token	*t;
	int		quotes;
	int		i;
	int		last_i;
	char	quotes_type;
}	t_parsing;

t_token	*last(t_token *start)
{
	if (!start)
		return (start);
	while (start->next)
		start = start->next;
	return (start);
}

int	ft_strlen(char *s)
{
	int	i;

	i = 0;
	while (s && s[i])
		i++;
	return (i);
}

void	add_back(t_token **start, t_token *new)
{
	t_token	*tmp;

	tmp = last(*start);
	tmp->next = new;
}

char	*substr(char *s, int start, int end)
{
	char	*res;
	int		i;

	i = 0;
	if (start > end || end > ft_strlen(s) || start > ft_strlen(s))
		return (NULL);
	res = malloc(sizeof(char) * (end - start + 1));
	if (!res)
		return (NULL);
	while (start + i < end)
	{
		res[i] = (s + start)[i];
		i++;
	}
	res[end - start] = 0;
	return (res);
}

t_token	*new_token(char *s, t_type genre)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	new->s = s;
	new->genre = genre;
	new->next = NULL;
	return (new);
}

char	*get_token(int n)
{
	if (n == 0)
		return ("REDIR");
	else if (n == 1)
		return ("CMD");
	else if (n == 2)
		return ("ELEMENT");
	else if (n == 3)
		return ("ELEM_S_QUOTES");
	else if (n == 4)
		return ("ELEM_D_QUOTES");
	else if (n == 5)
		return ("FLAGS");
	else if (n == 6)
		return ("FILENAME");
	else
		return ("NONE");
}

int	cut(t_parsing *p, char *s, t_type type)
{
	char	c;

	c = s[p->i];
	p->last_i = p->i;
	while (s[p->i] && s[p->i] == ' ')
		p->i++;
	while (s[p->i] && s[p->i] != ' ' && s[p->i] != '>'
		&& s[p->i] != '<' && s[p->i] != '|')
		p->i++;
	if (c == '<' || c == '>')
		while (s[p->i] && (s[p->i] == '>' || s[p->i] == '<'))
			p->i++;
	add_back(&p->t, new_token((substr(s, p->last_i, p->i)), type));
	p->last_i = p->i;
	if (s[p->i] == ' ')
		p->i--;
	return (1);
}

int	add_quotes(t_parsing *p, char *s)
{
	p->quotes++;
	p->quotes_type = s[p->i];
	return (1);
}

int	add_quote_content(t_parsing *p, char *s)
{
	p->last_i = p->i;
	while (s[p->i] && s[p->i] != p->quotes_type)
		p->i++;
	if (p->quotes_type == '\'')
		add_back(&p->t, new_token(substr(s, p->last_i, p->i), ELEM_S_QUOTES));
	else
		add_back(&p->t, new_token(substr(s, p->last_i, p->i), ELEM_D_QUOTES));
	if (s[p->i])
		p->quotes = 0;
	p->quotes_type = 0;
	return (1);
}

void	init_parsing(t_parsing *p)
{
	p->t = new_token("", ELEMENT);
	p->quotes_type = 0;
	p->last_i = 0;
	p->i = -1;
	p->quotes = 0;
}

void	parsing(char *s, t_parsing *p)
{
	while (s[++p->i])
	{
		if (!p->quotes)
		{
			if ((s[p->i] == '>' || s[p->i] == '<') && cut(p, s, REDIR))
				continue ;
			if ((s[p->i] == '\"' || s[p->i] == '\'') && add_quotes(p, s))
				continue ;
			if ((p->last_i == 0 || (s[p->i] == '|' && ++p->i))
				&& cut(p, s, CMD))
				continue ;
			if (!last(p->t)->genre && cut(p, s, ELEMENT))
				continue ;
			if (s[p->i] == '-' && cut(p, s, FLAGS))
				continue ;
			if (s[p->i] != ' ' && s[p->i] != '\'' && s[p->i] != '\"'
				&& s[p->i] != '>' && s[p->i] != '<' && s[p->i] != '|'
				&& cut(p, s, ELEMENT))
				continue ;
		}
		if (p->quotes)
			add_quote_content(p, s);
	}
}

t_token	*parse(char *s)
{
	t_parsing	p;

	init_parsing(&p);
	parsing(s, &p);
	if (p.quotes)
		return (NULL);
	return (p.t->next);
}

char	*add_space(char *s)
{
	char	*res;
	int		i;

	i = 0;
	res = malloc(sizeof(char) * (ft_strlen(s) + 2));
	if (!res)
		return (NULL);
	while (s && *s)
		res[i++] = *s++;
	res[i++] = ' ';
	res[i] = 0;
	return (res);
}

int	main(void)
{
	char	*line;
	char	*better_line;

	line = readline("shell>");
	better_line = add_space(line);
	free(line);
	parse(better_line);
}
